#include "Scripting/ScriptEngine.hpp"
#include "Core/Memory/Pointers.hpp"
#include "Scene/Module.hpp"

void ScriptEngine::init()
{
	
}

void ScriptEngine::onComponentAdded(const EC::ComponentPtr& component)
{
    EC::TComponentPtr<Script> script = component;
    CHECK_MSG(script.isValid(), "Trying to add a not valid Script derived component.");
    mScripts.push_back(script);
}

void ScriptEngine::update()
{
	PROFILER_CPU()

    std::vector<EC::TComponentPtr<Script>> newList;
    FOR_ARRAY(i, mScripts)
    {
        PROFILER_CPU_NAMED(removeScripts);

        EC::TComponentPtr<Script> script = mScripts[i];
        if(script.isValid())
        {
            newList.push_back(script);
        }
    }

    mScripts.clear();
    mScripts = newList;

    FOR_ARRAY(i, mScripts)
	{
        EC::TComponentPtr<Script> script = mScripts[i];
        if (script->isActive())
        {
            if (!script->getFirstUpdateDone())
            {
                script->firstUpdate();
                script->firstUpdateDone();
            }

            script->update();
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