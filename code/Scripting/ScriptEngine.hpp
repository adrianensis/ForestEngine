#pragma once

#include "Core/Module.hpp"
#include "Core/Systems/System.hpp"
#include "Scripting/Script.hpp"

class ScriptEngine: public System
{
	

public:
    void init();
    void addComponent(Ptr<SystemComponent> component) override;
    void update();
    void preSceneChanged();
    void postSceneChanged();
    void terminate();

private:
    void retrieveControllerFromScene();

private:
	std::vector<Ptr<Script>> mScripts;
	Ptr<Script> mController;
};
REGISTER_CLASS(ScriptEngine);
