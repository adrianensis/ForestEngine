#pragma once

#include "Engine/Core.hpp"
#include "Scripting/Script.hpp"
#include "Engine/EntityComponent/Component.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"

class ScriptEngine: public System::System, public EC::IComponentsListener
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void onComponentAdded(const EC::ComponentPtr& component) override;
    void update();
    void preSceneChanged();
    void postSceneChanged();

private:
	std::vector<EC::TComponentPtr<Script>> mScripts;
};
REGISTER_CLASS(ScriptEngine);
