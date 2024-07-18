#pragma once

#include "Core/ECS/System.hpp"
#include "Core/Config/ConfigObject.hpp"

class EngineConfig: public System
{
	
public:
    void init() override;

private:
	ConfigObject mConfig;

public:
	CRGET(Config)
};
REGISTER_CLASS(EngineConfig);
