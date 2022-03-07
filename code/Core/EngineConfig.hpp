#pragma once

#include "Core/Config/ConfigObject.hpp"
#include "Core/Singleton.hpp"

class EngineConfig: public ObjectBase, public Singleton<EngineConfig>
{
	GENERATE_METADATA(EngineConfig)
private:
	ConfigObject mConfig;

public:
	void init();

	CRGET(Config)
};