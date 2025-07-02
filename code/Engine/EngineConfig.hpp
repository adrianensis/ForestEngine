#pragma once

#include "Engine/System/System.hpp"
#include "Engine/Config/Config.hpp"

class EngineConfig: public System::System
{
	
public:
    void init() override;

private:
	Config::Config mConfig;

public:
	CRGET(Config)
};
REGISTER_CLASS(EngineConfig);
