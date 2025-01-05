#pragma once

#include "Core/Std.hpp"
class Config
{
public:
	const JSON& at(const std::string& key) const { return mJson.at(key); }
    void readFromJsonFile(const std::string& path);
    void writeToJsonFile(const std::string& path);
    bool contains(const std::string& key) const;

private:
	JSON mJson;
public:
	CRGET_SET(Json)
};
