#pragma once

#include "Core/Config/ConfigObject.hpp"
#include "Core/Singleton.hpp"

#ifdef CPP_INCLUDE
#include "Core/EngineConfig.hpp"
#endif

class EngineConfig: public ObjectBase, public Singleton<EngineConfig>
{
	GENERATE_METADATA(EngineConfig)
private:
	ConfigObject mConfig;

public:
	CPP void init()
	{
		mConfig.readFromJsonFile("config/engine.json");
	}

	CRGET(Config)
};