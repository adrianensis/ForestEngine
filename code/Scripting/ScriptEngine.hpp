#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Scripting/Script.hpp"
#include "Core/ECS/ComponentHandler.hpp"
#include "Core/ECS/ComponentsManager.hpp"

class ScriptEngine: public System, public IComponentsListener
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void onComponentAdded(const ComponentHandler& component) override;
    void update();
    void preSceneChanged();
    void postSceneChanged();

private:
    void retrieveControllerFromScene();

private:
	std::vector<TComponentHandler<Script>> mScripts;
	TComponentHandler<Script> mController;
};
REGISTER_CLASS(ScriptEngine);
