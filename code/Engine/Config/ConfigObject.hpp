#pragma once

#include "Core/Std.hpp"

/*
	example for constant config object:

	ConfigObject const conf;
	conf.setF32("key", 1.0f);
*/

class ConfigObject
{
public:
	const JSON& at(const std::string& key) const { return mJson.at(key); }
    void readFromJsonFile(const std::string& path);
    void writeToJsonFile(const std::string& path);
    void init();
    bool contains(const std::string& key) const;

private:
	JSON mJson;
public:
	CRGET_SET(Json)
};
