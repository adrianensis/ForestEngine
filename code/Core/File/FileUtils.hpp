#pragma once

#include "Core/Module.hpp"

class FileUtils
{
public:
    static void init();
    static void readFile(const std::string& path, std::function<void(std::ifstream& file)> callback);
    static void writeFile(const std::string& path, std::function<void(std::ofstream& file)> callback);
};
