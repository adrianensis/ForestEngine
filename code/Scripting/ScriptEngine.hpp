#pragma once

#include "Core/Module.hpp"
#include "Scripting/Script.hpp"

#ifdef CPP_INCLUDE
#include "Scripting/ScriptEngine.hpp"
#include "Scene/Module.hpp"
#endif

class ScriptEngine: public IEngineSystem, public Singleton<ScriptEngine>
{
	GENERATE_METADATA(ScriptEngine)

public:

	CPP void init()
	{
		TRACE()

		REGISTER_COMPONENT_CLASS_IN_ENGINE_SYSTEM(Script);

		mController = ScenesManager::getInstance().getGameObjectController()->getFirstComponent<Script>();
	}

	CPP void addComponent(Ptr<IEngineSystemComponent> component)
	{
		if(component.get().getClassId() == Script::getClassIdStatic())
		{
			Ptr<Script> script = Ptr<Script>::Cast(component);
			mScripts.push_back(script);
		}
	}

	CPP void update()
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

	CPP void terminate()
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

private:
	CPP void internalRemoveScript(std::list<Ptr<Script>>::iterator & it)
	{
		Ptr<Script> script = *it;

		script.get().terminate();
		script.get().finallyDestroy();
		//DELETE(script);

		it = mScripts.erase(it);
		--it;
	}

private:
	std::list<Ptr<Script>> mScripts;
	Ptr<Script> mController;
};