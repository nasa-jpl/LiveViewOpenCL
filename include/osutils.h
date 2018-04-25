#ifndef OSUTILS_H
#define OSUTILS_H

#include <dirent.h>
#include <sys/stat.h>

#include <vector>
#include <string>

namespace os
{
    void listdir(std::vector<std::string> &out, const std::string &directory);
    std::string getext(const std::string &f);
}


#endif // OSUTILS_H
