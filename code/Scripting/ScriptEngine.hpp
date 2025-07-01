#pragma once

#include "Core/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Scripting/Script.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"

class ScriptEngine: public System::System, public IComponentsListener
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void onComponentAdded(const ComponentPtr& component) override;
    void update();
    void preSceneChanged();
    void postSceneChanged();

private:
	std::vector<TComponentPtr<Script>> mScripts;
};
REGISTER_CLASS(ScriptEngine);
