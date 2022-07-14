#include "Core/Config/ConfigObject.hpp"

#include "Core/File/File.hpp"


void ConfigObject::readFromJsonFile(CR(std::string) path)
{
	File::readFile(path, [&](R(std::ifstream) file)
	{
		file >> mJson;
	});
}

void ConfigObject::writeToJsonFile(CR(std::string) path)
{
	File::writeFile(path, [&](R(std::ofstream) file)
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

bool ConfigObject::contains(R(std::string) key) const
{
	return mJson.contains(key);
}
