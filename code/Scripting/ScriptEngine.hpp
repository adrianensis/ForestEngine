#pragma once

#include "Core/Module.hpp"
#include "Scripting/Script.hpp"

class ScriptEngine: public IEngineSystem, public Singleton<ScriptEngine>
{
	GENERATE_METADATA(ScriptEngine)
	
private:
	std::list<Ptr<Script>> mScripts;
	Ptr<Script> mController;

	void internalRemoveScript(std::list<Ptr<Script>>::iterator & it);

public:
	void init();
	void addComponent(Ptr<IEngineSystemComponent> component) override;
	void update();
	void terminate();
};