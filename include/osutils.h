#ifndef OSUTILS_H
#define OSUTILS_H

#include <dirent.h>
#include <sys/stat.h>

#include <vector>
#include <regex>
#include <string>

namespace os
{
    void listdir(std::vector<std::string> &out, const std::string &directory);
    std::string getext(const std::string &f);
    std::string trim(const std::string &value);
}


#endif // OSUTILS_H
