#include "Core/File/FileUtils.hpp"
#include "Core/Log/Log.hpp"

void FileUtils::init() {

}
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
