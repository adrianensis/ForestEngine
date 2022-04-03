#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Serialization.hpp"

#ifdef CPP_INCLUDE
#include "Core/Config/ConfigObject.hpp"

#include "Core/File/File.hpp"
#endif


/*
	example for constant config object:

	ConfigObject const conf;
	conf.setF32("key", 1.0f);
*/

class ConfigObject: public ObjectBase
{
    GENERATE_METADATA(ConfigObject)
private:
	JSON mJson;

public:
	// void init();
	// void clear();
	// bool contains(std::string & key) const;

	// void readFromJsonFile(const std::string& path);
	// void writeToJsonFile(const std::string& path);

	const JSON& at(const std::string& key) const { return mJson.at(key); }

	CRGET_SET(Json)

	CPP void readFromJsonFile(const std::string& path)
	{
		File::readFile(path, [&](std::ifstream& file)
		{
			file >> mJson;
		});
	}

	CPP void writeToJsonFile(const std::string& path)
	{
		File::writeFile(path, [&](std::ofstream& file)
		{
			file << mJson.dump(4);
			// TODO : use dump(4) only for debug and release? but dump() for ship version?
		});
	}

	CPP void init()
	{
	}

	CPP void clear()
	{
		mJson.clear();
	}

	CPP bool contains(std::string& key) const
	{
		return mJson.contains(key);
	}
};