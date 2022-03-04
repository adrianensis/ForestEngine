#pragma once

#include "Core/Module.hpp"
#include "Scripting/Script.hpp"

class ScriptEngine: public IEngineSystem, public Singleton<ScriptEngine>
{
	GENERATE_METADATA(ScriptEngine)
	
private:
	std::list<Script *> mScripts;
	Script* mController = nullptr;

	void internalRemoveScript(std::list<Script *>::iterator & it);

public:
	void init();
	void addComponent(IEngineSystemComponent *component) override;
	void update();
	void terminate();
};