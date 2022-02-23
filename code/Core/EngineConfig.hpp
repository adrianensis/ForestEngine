#pragma once

#include "Core/Config/ConfigObject.hpp"
#include "Core/Singleton.hpp"

class EngineConfig: public ObjectBase, public Singleton<EngineConfig>
{
	GENERATE_METADATA(EngineConfig)
	PRI ConfigObject mConfig; GET_RC(Config)

public:
	void init();
};