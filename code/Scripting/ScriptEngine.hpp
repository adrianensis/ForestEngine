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
    void internalRemoveScript(std::list<Ptr<Script>>::iterator & it);
    void retrieveControllerFromScene();

private:
	std::list<Ptr<Script>> mScripts;
	Ptr<Script> mController;
};
