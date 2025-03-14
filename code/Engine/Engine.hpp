#pragma once

#include "Core/Minimal.hpp"

class Engine
{

public:
    void init();
    void preSceneChanged();
    void postSceneChanged();
    void run();
    void terminate();

private:
	f32 mFPS = 0.0f;
};
REGISTER_CLASS(Engine);
