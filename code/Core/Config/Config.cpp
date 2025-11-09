#include "Core/Config/Config.hpp"
#include "Core/File/FileUtils.hpp"

NS_BEGIN(Config)

void Config::readFromJsonFile(const std::string& path)
{
    mJson.clear();
	Core::FileUtils::readFile(path, [&](std::ifstream& file)
	{
		file >> mJson;
	});
}

void Config::writeToJsonFile(const std::string& path)
{
	Core::FileUtils::writeFile(path, [&](std::ofstream& file)
	{
		file << mJson.dump(4);
	});
}

bool Config::contains(const std::string& key) const
{
	return mJson.contains(key);
}

NS_END