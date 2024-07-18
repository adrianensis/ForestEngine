#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/System.hpp"
#include "Scripting/Script.hpp"
#include "Core/ECS/ComponentHandler.hpp"

class ScriptEngine: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void addSystemComponent(const ComponentHandler& component) override;
    virtual void update() override;
    void preSceneChanged();
    void postSceneChanged();

private:
    void retrieveControllerFromScene();

private:
	std::vector<TypedComponentHandler<Script>> mScripts;
	TypedComponentHandler<Script> mController;
};
REGISTER_CLASS(ScriptEngine);
