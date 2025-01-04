#pragma once

#include "Core/System/System.hpp"
#include "Core/Config/Config.hpp"

class EngineConfig: public System
{
	
public:
    void init() override;

private:
	Config mConfig;

public:
	CRGET(Config)
};
REGISTER_CLASS(EngineConfig);
