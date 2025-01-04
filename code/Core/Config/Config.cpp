#include "Core/Config/Config.hpp"
#include "Core/File/FileUtils.hpp"

void Config::readFromJsonFile(const std::string& path)
{
    mJson.clear();
	FileUtils::readFile(path, [&](std::ifstream& file)
	{
		file >> mJson;
	});
}

void Config::writeToJsonFile(const std::string& path)
{
	FileUtils::writeFile(path, [&](std::ofstream& file)
	{
		file << mJson.dump(4);
	});
}

bool Config::contains(const std::string& key) const
{
	return mJson.contains(key);
}
