#pragma once

#include "Engine/Systems/System.hpp"
#include "Assets/Config/ConfigObject.hpp"

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
