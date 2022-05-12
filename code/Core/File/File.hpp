#ifndef FILE_HPP
#define FILE_HPP

#include "Core/Module.hpp"

class File
{
public:
    static void init();
    static void readFile(const std::string& path, std::function<void(std::ifstream& file)> callback);
    static void writeFile(const std::string& path, std::function<void(std::ofstream& file)> callback);
};

#endif