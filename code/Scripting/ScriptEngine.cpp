#include "Scripting/ScriptEngine.hpp"
#include "Scene/Module.hpp"


void ScriptEngine::init()
{
	LOG_TRACE()

	registerComponentClass(Script::getClassDefinitionStatic().mId);

    if(GET_SYSTEM(ScenesManager).getGameObjectController())
    {
	    mController = GET_SYSTEM(ScenesManager).getGameObjectController()->getFirstComponent<Script>();
    }
}

void ScriptEngine::addComponent(Ptr<SystemComponent> component)
{
	System::addComponent(component);

    Ptr<Script> script = Ptr<Script>::cast(component);
    CHECK_MSG(script.isValid(), "Trying to add a not valid Script derived component.");
    mScripts.push_back(script);
}

void ScriptEngine::update()
{
	PROFILER_CPU()
	
	if (mController)
	{
		if (!mController->getFirstUpdateDone())
		{
			mController->firstUpdate();
			mController->firstUpdateDone();
		}

		mController->update();
	}

	FOR_LIST(it, mScripts)
	{
		Ptr<Script> script = *it;

		if (script->isActive())
		{
			if (!script->getFirstUpdateDone())
			{
				script->firstUpdate();
				script->firstUpdateDone();
			}

			script->update();
		}
		else if (script->getIsPendingToBeDestroyed())
		{
			internalRemoveScript(it);
		}
	}

}

void ScriptEngine::terminate()
{
	LOG_TRACE()


}

void ScriptEngine::internalRemoveScript(std::list<Ptr<Script>>::iterator & it)
{
	Ptr<Script> script = *it;

	script->terminate();
	script->finallyDestroy();

	it = mScripts.erase(it);
	--it;
}
