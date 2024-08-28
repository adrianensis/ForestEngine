#include "Core/File/FileUtils.hpp"
#include "Core/Assert/Assert.hpp"
#include <iostream>

void FileUtils::readFile(const std::string& path, std::function<void(std::ifstream& file)> callback)
{
	std::ifstream file;
	file.open(path);

    CHECK_MSG(file.good() && !file.fail(), "Couldn't open " + path);

	if (callback)
    {
        callback(file);
    }

	file.close();
}

void FileUtils::readFileBinaryData(const std::string& path, std::vector<byte>& output)
{
	std::ifstream file;
    // std::ios::ate -> seek to the end of stream immediately after open 
	file.open(path, std::ios::ate | std::ios_base::binary);

    CHECK_MSG(file.good() && !file.fail(), "Couldn't open " + path);

    // read the file size
    u32 length = file.tellg();
    // go back to beginning
    file.seekg(0, std::ios_base::beg);

    // Make a buffer of the exact size of the file and read the data into it.
    output.resize(length);
    file.read(reinterpret_cast<char*>(output.data()), length);
	file.close();
}

void FileUtils::writeFile(const std::string& path, std::function<void(std::ofstream& file)> callback)
{
    const std::filesystem::path fsPath{path};
    const std::filesystem::path fileName = fsPath.filename();
    CHECK_MSG(fsPath.has_filename(), "No filename found in " + path);
    CHECK_MSG(fsPath.has_parent_path(), "No parent path found in " + path);

    if(!std::filesystem::exists(fsPath.parent_path()))
    {
        std::filesystem::create_directories(fsPath.parent_path());
    }

	std::ofstream file;
	file.open(path);
    CHECK_MSG(file.good() && !file.fail(), "Couldn't open " + path);

    if (callback)
    {
        callback(file);
    }
	file.close();
}
