#pragma once

#include "Core/Module.hpp"
#include "Scripting/Script.hpp"


class ScriptEngine: public IEngineSystem, public Singleton<ScriptEngine>
{
	GENERATE_METADATA(ScriptEngine)

public:
    void init();
    void addComponent(Ptr<IEngineSystemComponent> component);
    void update();
    void terminate();

private:
    void internalRemoveScript(std::list<Ptr<Script>>::iterator & it);

private:
	std::list<Ptr<Script>> mScripts;
	Ptr<Script> mController;
};
