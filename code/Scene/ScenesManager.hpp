#pragma once

#include "Core/Module.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"

#ifdef CPP_INCLUDE
#include "ScenesManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Module.hpp"
#include "Core/EngineConfig.hpp"
#endif

class ScenesManager: public ObjectBase, public Singleton<ScenesManager>
{
	GENERATE_METADATA(ScenesManager)

public:
	CPP ScenesManager()
	{
		mCurrentScene = nullptr;
		mSceneHasChanged = true;
	}

	CPP ~ScenesManager() override
	{
		DELETE_CONTENT(mScenes)

		mGameObjectController->destroy();
		DELETE(mGameObjectController);
	}

	CPP void init()
	{
		mCurrentSceneIndex = 0;

		u32 scenesCount = EngineConfig::getInstance().getConfig().at("scenes").size();

		if (scenesCount == 0)
		{
			scenesCount = 1; // min 1 scene
		}

		FOR_RANGE(i, 0, scenesCount)
		{
			Scene *scene = NEW(Scene);
			addScene(scene);
		}

		mCurrentScene = *mScenes.begin();
	}

	CPP void update()
	{
		PROFILER_TIMEMARK_START()
		mCurrentScene->update();
		PROFILER_TIMEMARK_END()
	}

	CPP void loadCurrentScene()
	{
		if (mSceneHasChanged)
		{
			mCurrentScene = mScenes[mCurrentSceneIndex];
			internalLoadScene();
			mSceneHasChanged = false;
		}
	}

	CPP void setScene(u32 i)
	{
		if (mCurrentSceneIndex != i)
		{
			mCurrentSceneIndex = i;
			mSceneHasChanged = true;
		}
	}

private:
	CPP void internalLoadScene()
	{
		mCurrentScene->init();

		if (EngineConfig::getInstance().getConfig().at("scenes").size() > 0)
		{
			std::string sceneName = EngineConfig::getInstance().getConfig().at("scenes")[mCurrentSceneIndex].get<std::string>();

			mCurrentScene->loadScene(sceneName);
		}

		mGameObjectController->setScene(mCurrentScene);
		RenderEngine::getInstance().setCamera(mCurrentScene->getCameraGameObject()->getFirstComponent<Camera>());
	}


	CPP void addScene(Scene *newScene)
	{
		mScenes.push_back(newScene);
	}

private:
	std::vector<Scene *> mScenes;
	u32 mCurrentSceneIndex = 0;

	Scene* mCurrentScene = nullptr;
	bool mSceneHasChanged = false;
	GameObject* mGameObjectController = nullptr;

public:
	GET(CurrentScene)
	GET(SceneHasChanged)
	GET_SET(GameObjectController)
};