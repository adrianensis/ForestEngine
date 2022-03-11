#include "Scripting/ScriptEngine.hpp"

#include "Scripting/Script.hpp"
#include "Scene/Module.hpp"

void ScriptEngine::init()
{
	TRACE()

	REGISTER_COMPONENT_CLASS_IN_ENGINE_SYSTEM(Script);

	mController = ScenesManager::getInstance().getGameObjectController()->getFirstComponent<Script>();
}

void ScriptEngine::addComponent(Ptr<IEngineSystemComponent> component)
{
	if(component.get().getClassId() == Script::getClassIdStatic())
	{
		Ptr<Script> script = Ptr<Script>::Cast(component);
		mScripts.push_back(script);
	}
}

void ScriptEngine::update()
{
	PROFILER_TIMEMARK_START()
	
	if (mController)
	{
		if (!mController.get().getFirstUpdateDone())
		{
			mController.get().firstUpdate();
			mController.get().firstUpdateDone();
		}

		mController.get().update();
	}

	FOR_LIST(it, mScripts)
	{
		Ptr<Script> script = *it;

		if (script.get().isActive())
		{
			if (!script.get().getFirstUpdateDone())
			{
				script.get().firstUpdate();
				script.get().firstUpdateDone();
			}

			script.get().update();
		}
		else if (script.get().getIsPendingToBeDestroyed())
		{
			internalRemoveScript(it);
		}
	}

	PROFILER_TIMEMARK_END()
}

void ScriptEngine::internalRemoveScript(std::list<Ptr<Script>>::iterator& it)
{
	Ptr<Script> script = *it;

	script.get().terminate();
	script.get().finallyDestroy();
	//DELETE(script);

	it = mScripts.erase(it);
	--it;
}

void ScriptEngine::terminate()
{
	TRACE()

	// if (mController)
	// {
	// 	mController->terminate();
	// 	DELETE(mController);
	// }

	// FOR_LIST(it, mScripts)
	// {
	// 	Script *script = *it;
	// 	script->terminate();
	// 	DELETE(script);
	// }
}