#include "Core/Config/ConfigObject.hpp"

#include "Core/File/File.hpp"


void ConfigObject::readFromJsonFile(const std::string& path)
{
	File::readFile(path, [&](std::ifstream& file)
	{
		file >> mJson;
	});
}

void ConfigObject::writeToJsonFile(const std::string& path)
{
	File::writeFile(path, [&](std::ofstream& file)
	{
		file << mJson.dump(4);
	});
}

void ConfigObject::init()
{
}

void ConfigObject::clear()
{
	mJson.clear();
}

bool ConfigObject::contains(std::string& key) const
{
	return mJson.contains(key);
}
