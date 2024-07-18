#include "Scripting/ScriptEngine.hpp"
#include "Scene/Module.hpp"


void ScriptEngine::init()
{
	LOG_TRACE()

	registerComponentClass(ClassManager::getClassMetadata<Script>().mClassDefinition.getId());
}

void ScriptEngine::addSystemComponent(const ComponentHandler& component)
{
	System::addSystemComponent(component);

    TypedComponentHandler<Script> script = component;
    CHECK_MSG(script.isValid(), "Trying to add a not valid Script derived component.");
    mScripts.push_back(script);
}

void ScriptEngine::update()
{
	PROFILER_CPU()

    std::vector<TypedComponentHandler<Script>> newList;
    FOR_ARRAY(i, mScripts)
    {
        PROFILER_BLOCK_CPU("remove scripts");

        TypedComponentHandler<Script> script = mScripts[i];
        if(script.isValid())
        {
            newList.push_back(script);
        }
    }

    mScripts.clear();
    mScripts = newList;

    FOR_ARRAY(i, mScripts)
	{
        TypedComponentHandler<Script> script = mScripts[i];
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
	LOG_TRACE()
}

void ScriptEngine::preSceneChanged()
{
	LOG_TRACE()
    retrieveControllerFromScene();
}

void ScriptEngine::postSceneChanged()
{
	LOG_TRACE()
}

void ScriptEngine::retrieveControllerFromScene()
{
    if(GET_SYSTEM(ScenesManager).getGameObjectController())
    {
	    mController = GET_SYSTEM(ScenesManager).getGameObjectController()->getFirstComponent<Script>();
    }
}
