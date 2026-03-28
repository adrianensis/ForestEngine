#include "Scripting/ScriptEngine.hpp"
#include "Core/Memory/Pointers.hpp"
#include "Scene/Module.hpp"

void ScriptEngine::init()
{
	
}

void ScriptEngine::onComponentAdded(EC::Component* component)
{
    Script* script = CAST(Script, component);
    CHECK_MSG(script, "Trying to add a not valid Script derived component.");
    mScripts.push_back(script);
}

void ScriptEngine::update(Core::f32 dt)
{
	PROFILER_CPU()

    std::vector<Script*> newList;
    FOR_ARRAY(i, mScripts)
    {
        PROFILER_CPU_NAMED(removeScripts);

        Script* script = mScripts[i];
        if(script)
        {
            newList.push_back(script);
        }
    }

    mScripts.clear();
    mScripts = newList;

    FOR_ARRAY(i, mScripts)
	{
        Script* script = mScripts[i];
        if (script->isActive())
        {
            if (!script->getFirstUpdateDone())
            {
                script->firstUpdate(dt);
                script->firstUpdateDone();
            }

            script->update(dt);
        }
	}
}

void ScriptEngine::terminate()
{
}

void ScriptEngine::preSceneChanged()
{
}

void ScriptEngine::postSceneChanged()
{
}