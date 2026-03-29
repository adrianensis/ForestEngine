#pragma once

#include "Engine/Core.hpp"
#include "Core/Time/TimeUtils.hpp"
#include "Core/Time/TimerManager.hpp"
#include "Scene/ScenesManager.hpp"

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
    Time::Time mTime;
    Time::TimerManager mTimerManager;
    ScenesManager* mScenesManager = nullptr;

public:
    GET(ScenesManager)
};