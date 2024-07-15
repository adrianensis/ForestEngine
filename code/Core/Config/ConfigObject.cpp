#include "Core/Config/ConfigObject.hpp"
#include "Core/File/FileUtils.hpp"

void ConfigObject::readFromJsonFile(const std::string& path)
{
    mJson.clear();
	FileUtils::readFile(path, [&](std::ifstream& file)
	{
		file >> mJson;
	});
}

void ConfigObject::writeToJsonFile(const std::string& path)
{
	FileUtils::writeFile(path, [&](std::ofstream& file)
	{
		file << mJson.dump(4);
	});
}

void ConfigObject::init()
{
}

bool ConfigObject::contains(const std::string& key) const
{
	return mJson.contains(key);
}
