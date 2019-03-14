#include "osutils.h"

void os::listdir(std::vector<std::string> &out, const std::string &directory)
{
    DIR *dir;
    struct dirent *ent;
    struct stat st = {};

    if ((dir = opendir(directory.data())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            const std::string file_name = ent->d_name;
            const std::string full_file_name = directory + "/" + file_name;

            if (file_name[0] == '.')
                continue;

            if (stat(full_file_name.c_str(), &st) == -1)
                continue;

            const bool is_directory = (st.st_mode & S_IFDIR) != 0;

            if (is_directory)
                continue;

            out.push_back(full_file_name);
        }
        closedir(dir);
    } else {
        out.emplace_back("");
        perror("");
    }
}

std::string os::getext(const std::string &f)
{
    size_t sz = f.rfind('.', f.length());
    if (sz != std::string::npos)
        return (f.substr(sz + 1, f.length() - sz));

    return ("");
}

std::string os::trim(const std::string &value)
{
    return std::regex_replace(value, std::regex("^ +| +$|( ) +"), "$1");
}
