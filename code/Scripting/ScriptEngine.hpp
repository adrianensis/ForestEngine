#pragma once

#include "Core/Module.hpp"
#include "Scripting/Script.hpp"


class ScriptEngine: public System
{
	GENERATE_METADATA(ScriptEngine)

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
