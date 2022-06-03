#ifndef CONFIGOBJECT_HPP
#define CONFIGOBJECT_HPP

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Serialization.hpp"

/*
	example for constant config object:

	ConfigObject const conf;
	conf.setF32("key", 1.0f);
*/

class ConfigObject: public ObjectBase
{
    GENERATE_METADATA(ConfigObject)

public:
	const JSON& at(const String& key) const { return mJson.at(key); }
    void readFromJsonFile(const String& path);
    void writeToJsonFile(const String& path);
    void init();
    void clear();
    bool contains(String& key) const;

private:
	JSON mJson;
public:
	CRGET_SET(Json)
};

#endif