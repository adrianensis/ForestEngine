#include "Engine/EngineConfig.hpp"


void EngineConfig::init()
{
	mConfig.readFromJsonFile("config/engine.json");
}
