#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Scripting/Script.hpp"
#include "Core/EntityComponent/ComponentHandler.hpp"
#include "Core/EntityComponent/ComponentsManager.hpp"

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
	std::vector<TComponentHandler<Script>> mScripts;
};
REGISTER_CLASS(ScriptEngine);
