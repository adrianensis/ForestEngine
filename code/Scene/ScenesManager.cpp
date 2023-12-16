#include "ScenesManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Module.hpp"
#include "Engine/EngineConfig.hpp"


ScenesManager::ScenesManager()
{
	mSceneHasChanged = true;
}

ScenesManager::~ScenesManager() 
{
    terminate();
}

void ScenesManager::terminate() 
{
    if(mGameObjectController)
    {
	    mGameObjectController->destroy();
    }
    mGameObjectController.invalidate();
}

void ScenesManager::init()
{
	mCurrentSceneIndex = 0;

	u32 scenesCount = GET_SYSTEM(EngineConfig).getConfig().at("scenes").size();

	if (scenesCount == 0)
	{
		scenesCount = 1; // min 1 scene
	}

	FOR_RANGE(i, 0, scenesCount)
	{
		mScenes.emplace_back(OwnerPtr<Scene>::newObject());
	}

	mCurrentScene = *mScenes.begin();
}

void ScenesManager::update()
{
	PROFILER_CPU()
	mCurrentScene->update();
}

void ScenesManager::loadCurrentScene()
{
	if (mSceneHasChanged)
	{
		mCurrentScene = mScenes[mCurrentSceneIndex];
		internalLoadScene();
		mSceneHasChanged = false;
	}
}

void ScenesManager::setScene(u32 i)
{
	if (mCurrentSceneIndex != i)
	{
		mCurrentSceneIndex = i;
		mSceneHasChanged = true;
	}
}

void ScenesManager::internalLoadScene()
{
	mCurrentScene->init();

	if (GET_SYSTEM(EngineConfig).getConfig().at("scenes").size() > 0)
	{
		std::string sceneName = GET_SYSTEM(EngineConfig).getConfig().at("scenes")[mCurrentSceneIndex].get<std::string>();

		mCurrentScene->loadScene(sceneName);
	}

    if(mGameObjectController)
    {
	    mGameObjectController->mScene = (mCurrentScene);
    }
	GET_SYSTEM(RenderEngine).mCamera = (mCurrentScene->getCameraGameObject()->getFirstComponent<Camera>());
}
