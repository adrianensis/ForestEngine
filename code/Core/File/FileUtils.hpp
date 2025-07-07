#pragma once

#include "Core/StdCore.hpp"
#include <fstream>
#include <vector>
#include <functional>

NS_BEGIN(Core)
class FileUtils
{
public:
    static void readFile(const std::string& path, std::function<void(std::ifstream& file)> callback);
    static void readFileBinaryData(const std::string& path, std::vector<byte>& output);
    static void writeFile(const std::string& path, std::function<void(std::ofstream& file)> callback);
};
NS_END