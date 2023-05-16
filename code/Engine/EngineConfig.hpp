#pragma once

#include "Core/Config/ConfigObject.hpp"
#include "Engine/EngineSystem.hpp"

class EngineConfig: public EngineSystem
{
	GENERATE_METADATA(EngineConfig)
public:
    void init();

private:
	ConfigObject mConfig;

public:
	CRGET(Config)
};
