#pragma once

#include "Core/Systems/System.hpp"
#include "Core/Config/ConfigObject.hpp"

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
