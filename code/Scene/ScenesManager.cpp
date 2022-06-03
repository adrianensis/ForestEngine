#include "ScenesManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Module.hpp"
#include "Core/EngineConfig.hpp"


ScenesManager::ScenesManager()
{
	mCurrentScene = nullptr;
	mSceneHasChanged = true;
}

ScenesManager::~ScenesManager() 
{
	DELETE_CONTENT(mScenes)

	mGameObjectController->destroy();
	DELETE(mGameObjectController);
}

void ScenesManager::init()
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

void ScenesManager::update()
{
	PROFILER_TIMEMARK_START()
	mCurrentScene->update();
	PROFILER_TIMEMARK_END()
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

	if (EngineConfig::getInstance().getConfig().at("scenes").size() > 0)
	{
		std::string sceneName = EngineConfig::getInstance().getConfig().at("scenes")[mCurrentSceneIndex].get<std::string>();

		mCurrentScene->loadScene(sceneName);
	}

	mGameObjectController->setScene(mCurrentScene);
	RenderEngine::getInstance().setCamera(mCurrentScene->getCameraGameObject()->getFirstComponent<Camera>());
}

void ScenesManager::addScene(Scene *newScene)
{
	mScenes.push_back(newScene);
}
