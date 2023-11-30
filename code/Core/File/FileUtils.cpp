#include "Core/File/FileUtils.hpp"
#include "Core/Log/Log.hpp"

void FileUtils::init() {

}
void FileUtils::readFile(const std::string& path, std::function<void(std::ifstream& file)> callback)
{
	std::ifstream file;
	file.open(path);

	if(file.good() && !file.fail())
	{
		if (callback)
		{
			callback(file);
		}
	}

	file.close();
}

void FileUtils::writeFile(const std::string& path, std::function<void(std::ofstream& file)> callback)
{
	std::ofstream file;
	file.open(path);

	if(file.good() && !file.fail())
	{
		if (callback)
		{
			callback(file);
		}
	}

	file.close();
}
