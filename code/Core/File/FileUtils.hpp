#pragma once

#include "Core/StdPrimitiveTypes.hpp"
#include <filesystem>
#include <fstream>

class FileUtils
{
public:
    static void readFile(const std::string& path, std::function<void(std::ifstream& file)> callback);
    static void writeFile(const std::string& path, std::function<void(std::ofstream& file)> callback);
};
