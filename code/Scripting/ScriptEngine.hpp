#pragma once

#include "Core/Module.hpp"
#include "Scripting/Script.hpp"

class ScriptEngine: public IEngineSystem, public Singleton<ScriptEngine>
{
	GENERATE_METADATA(ScriptEngine)
	
private:
	std::list<Ref<Script>> mScripts;
	Ref<Script> mController;

	void internalRemoveScript(std::list<Ref<Script>>::iterator & it);

public:
	void init();
	void addComponent(Ref<IEngineSystemComponent> component) override;
	void update();
	void terminate();
};