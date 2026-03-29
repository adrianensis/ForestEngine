#pragma once

#include "Core/System/System.hpp"
#include "Core/Config/Config.hpp"

class EngineConfig: public System::System
{
	
public:
    void init();

private:
	Config::Config mConfig;

public:
	CRGET(Config)
};
REGISTER_CLASS(EngineConfig, System);
