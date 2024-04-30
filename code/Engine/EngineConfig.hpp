#pragma once

#include "Engine/System/System.hpp"
#include "Engine/Config/ConfigObject.hpp"

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
