#ifndef FILE_HPP
#define FILE_HPP

#include "Core/Module.hpp"

class File
{
public:
    static void init();
    static void readFile(CR(std::string) path, std::function<void(R(std::ifstream) file)> callback);
    static void writeFile(CR(std::string) path, std::function<void(R(std::ofstream) file)> callback);
};

#endif