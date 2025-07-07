#pragma once

#include "Core/Std.hpp"
#include "Core/JSON/JSON.hpp"

NS_BEGIN(Config)

class Config
{
public:
	const Core::JSON& at(const std::string& key) const { return mJson.at(key); }
    void readFromJsonFile(const std::string& path);
    void writeToJsonFile(const std::string& path);
    bool contains(const std::string& key) const;

private:
	Core::JSON mJson;
public:
	CRGET_SET(Json)
};

NS_END