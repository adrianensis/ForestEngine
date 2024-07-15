#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Scripting/Script.hpp"

class ScriptEngine: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void addSystemComponent(Ptr<SystemComponent> component) override;
    virtual void update() override;
    void preSceneChanged();
    void postSceneChanged();

private:
    void retrieveControllerFromScene();

private:
	std::vector<Ptr<Script>> mScripts;
	Ptr<Script> mController;
};
REGISTER_CLASS(ScriptEngine);
