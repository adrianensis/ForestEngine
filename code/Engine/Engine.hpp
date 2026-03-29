#pragma once

#include "Engine/Core.hpp"

class Engine
{

public:
    void init();
    void preSceneChanged();
    void postSceneChanged();
    void run();
    void terminate();

private:
	Core::f32 mFPS = 0.0f;
};