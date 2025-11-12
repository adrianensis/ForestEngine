#pragma once

#include "Engine/Core.hpp"
#include "Scripting/Script.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"

class ScriptEngine: public System::System, public EC::IComponentsListener
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void onComponentAdded(const EC::ComponentPtrBase& component) override;
    void update();
    void preSceneChanged();
    void postSceneChanged();

private:
	std::vector<EC::ComponentPtr<Script>> mScripts;
};
REGISTER_CLASS(ScriptEngine);
