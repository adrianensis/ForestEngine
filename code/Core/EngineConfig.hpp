#pragma once

#include "Core/Config/ConfigObject.hpp"
#include "Core/Singleton.hpp"

class EngineConfig: public ObjectBase, public Singleton<EngineConfig>
{
	GENERATE_METADATA(EngineConfig)
public:
    void init();

private:
	ConfigObject mConfig;

public:
	CRGET(Config)
};
