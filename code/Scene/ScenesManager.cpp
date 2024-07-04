#include "ScenesManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Module.hpp"
#include "Engine/EngineConfig.hpp"

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

	if (mCameraGameObject)
	{
		Ptr<Camera> cameraComponent = mCameraGameObject->getFirstComponent<Camera>();
		mCameraGameObject->removeComponent(cameraComponent);
		mCameraGameObject->destroy();
        mCameraGameObject.invalidate();
	}

    FOR_MAP(it, mScenes)
    {
        it->second->terminate();
    }

    mScenes.clear();
}

void ScenesManager::init()
{
    mScenes.emplace(smDefaultSceneName, OwnerPtr<Scene>::newObject());
    mScenes.emplace(smDefaultUISceneName, OwnerPtr<Scene>::newObject());

    mScenes.at(smDefaultSceneName)->init(smDefaultSceneName);
    mScenes.at(smDefaultUISceneName)->init(smDefaultUISceneName);

    requestLoadScene(smDefaultSceneName);
    requestLoadScene(smDefaultUISceneName);

    GameObjectHandler cameraGameObjectHandler = GET_SYSTEM(GameObjectsManager).requestGameObject<GameObject>();
    mCameraGameObject = cameraGameObjectHandler.getGameObject();
	mCameraGameObject->init();

	mCameraGameObject->mTransform->setLocalPosition(Vector3(0, 0, 10.0f));

    Ptr<Camera> cameraComponent = mCameraGameObject->createComponent<Camera>();
	cameraComponent->setPerspective(0.1, 10000, GET_SYSTEM(WindowManager).getMainWindow()->getAspectRatio(), 90);

    GET_SYSTEM(CameraManager).setCamera(cameraComponent);
}

void ScenesManager::update()
{
	PROFILER_CPU()
    
    Ptr<Camera> cameraComponent = mCameraGameObject->getFirstComponent<Camera>();
    cameraComponent->update();

    FOR_MAP(it, mLoadedScenes)
    {
        it->second->update();
    }
}

void ScenesManager::loadPendingScenes()
{
    FOR_LIST(it, mLoadRequests)
    {
        HashedString sceneName = *it;
        if(mScenes.contains(sceneName) && !mLoadedScenes.contains(sceneName))
        {
            internalLoadScene(sceneName);
        }
    }
    mLoadRequests.clear();
}

bool ScenesManager::pendingLoadRequests() const
{
	return !mLoadRequests.empty();
}

void ScenesManager::requestLoadScene(HashedString sceneName)
{
	mLoadRequests.insert(sceneName);
}

Ptr<Scene> ScenesManager::getScene(HashedString sceneName) const
{
    return mScenes.at(sceneName);
}

void ScenesManager::internalLoadScene(HashedString sceneName)
{
	mLoadedScenes.insert_or_assign(sceneName, mScenes.at(sceneName));
    mLoadedScenes.at(sceneName)->loadScene();
}
