#pragma once

#include "Core/Systems/System.hpp"
#include "Core/Config/ConfigObject.hpp"

class EngineConfig: public System
{
	
public:
    void init();

private:
	ConfigObject mConfig;

public:
	CRGET(Config)
};
REGISTER_CLASS(EngineConfig);
