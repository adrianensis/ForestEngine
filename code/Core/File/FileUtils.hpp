#pragma once

#include "Core/Std.hpp"
#include <fstream>

class FileUtils
{
public:
    static void readFile(const std::string& path, std::function<void(std::ifstream& file)> callback);
    static void readFileBinaryData(const std::string& path, std::vector<byte>& output);
    static void writeFile(const std::string& path, std::function<void(std::ofstream& file)> callback);
};
