#include "Scripting/ScriptEngine.hpp"
#include "Scene/Module.hpp"


void ScriptEngine::init()
{
	TRACE()

    IEngineSystem::init();

	REGISTER_COMPONENT_CLASS_IN_ENGINE_SYSTEM(Script);

	mController = ScenesManager::getInstance().getGameObjectController().get().getFirstComponent<Script>();
}

void ScriptEngine::addComponent(Ptr<IEngineSystemComponent> component)
{
	if(component.get().getClassId() == Script::getClassIdStatic())
	{
		Ptr<Script> script = Ptr<Script>::cast(component);
		mScripts.push_back(script);
	}
}

void ScriptEngine::update()
{
	PROFILER_CPU()
	
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

}

void ScriptEngine::terminate()
{
	TRACE()


}

void ScriptEngine::internalRemoveScript(std::list<Ptr<Script>>::iterator & it)
{
	Ptr<Script> script = *it;

	script.get().terminate();
	script.get().finallyDestroy();

	it = mScripts.erase(it);
	--it;
}
