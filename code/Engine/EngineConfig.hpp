#pragma once

#include "Core/System/System.hpp"
#include "Core/Config/Config.hpp"

class EngineConfig: public Core::System
{
	
public:
    void init() override;

private:
	Core::Config mConfig;

public:
	CRGET(Config)
};
REGISTER_CLASS(EngineConfig);
