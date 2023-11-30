#pragma once

#include "Core/Config/ConfigObject.hpp"
#include "Core/Systems/System.hpp"

class EngineConfig: public System
{
	GENERATE_METADATA(EngineConfig)
public:
    void init();

private:
	ConfigObject mConfig;

public:
	CRGET(Config)
};
