#pragma once

#include "Core/Module.hpp"

class Engine: public ObjectBase, public Singleton<Engine>
{
	GENERATE_METADATA(Engine)

public:
    void init();
    void preSceneChanged();
    void postSceneChanged();
    void run();
    void terminate();

private:
	f32 mFPS = 0.0f;
};
