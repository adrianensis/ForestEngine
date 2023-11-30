#pragma once

#include "Core/Config/ConfigObject.hpp"
#include "Engine/System.hpp"

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
