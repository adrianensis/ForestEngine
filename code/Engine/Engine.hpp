#pragma once

#include "Core/Object/ObjectBase.hpp"
#include "Core/Object/Singleton.hpp"

class Engine: public ObjectBase, public Singleton<Engine>
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
