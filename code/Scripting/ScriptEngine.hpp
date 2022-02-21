#pragma once

#include "Core/Module.hpp"
#include "Scripting/Script.hpp"

class ScriptEngine: public ISubsystem, public Singleton<ScriptEngine>
{
	GENERATE_METADATA(ScriptEngine)
	PRI std::list<Script *> mScripts;
	PRI Script* mController = nullptr;

	void internalRemoveScript(std::list<Script *>::iterator & it);

PUB
	void init();
	void addComponent(Component *component) override;
	void update();
	void terminate();
};