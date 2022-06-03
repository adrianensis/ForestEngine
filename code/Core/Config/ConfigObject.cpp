#include "Core/Config/ConfigObject.hpp"

#include "Core/File/File.hpp"


void ConfigObject::readFromJsonFile(const String& path)
{
	File::readFile(path, [&](std::ifstream& file)
	{
		file >> mJson;
	});
}

void ConfigObject::writeToJsonFile(const String& path)
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

bool ConfigObject::contains(String& key) const
{
	return mJson.contains(key);
}
