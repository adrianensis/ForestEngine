#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/Systems/System.hpp"
#include "Scripting/Script.hpp"

class ScriptEngine: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void addComponent(Ptr<SystemComponent> component) override;
    void update();
    void preSceneChanged();
    void postSceneChanged();

private:
    void retrieveControllerFromScene();

private:
	std::vector<Ptr<Script>> mScripts;
	Ptr<Script> mController;
};
REGISTER_CLASS(ScriptEngine);
