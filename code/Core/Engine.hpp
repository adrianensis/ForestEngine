#pragma once

#include "Core/Module.hpp"
class Engine: public ObjectBase, public Singleton<Engine>
{
	GENERATE_METADATA(Engine)
	PRI f32 mFPS = 0.0f;

PUB
	void init();
	void initSubsystems();
	void terminateSubSystems();
	void run();
	void terminate();
};