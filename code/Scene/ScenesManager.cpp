#include "ScenesManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneObject.hpp"
#include "Graphics/Module.hpp"
#include "Engine/EngineConfig.hpp"

void ScenesManager::terminate() 
{
    if(mSceneObjectController)
    {
	    mSceneObjectController->destroy();
    }
    mSceneObjectController.reset();

	if (mCameraSceneObject)
	{
        TComponentPtr<Camera> cameraComponent = EC.getFirstComponent<Camera>(mCameraSceneObject);
        EC.removeComponent(mCameraSceneObject, cameraComponent);
		mCameraSceneObject->destroy();
        mCameraSceneObject.reset();
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

    mCameraSceneObject = EC.requestEntity<SceneObject>();
	mCameraSceneObject->init();

	// mCameraSceneObject->mTransform->setLocalPosition(Vector3(0, 0, 10));
	mCameraSceneObject->mTransform->setLocalPosition(Vector3(0, 0, 0.3f));

    TComponentPtr<Camera> camera = EC.requestComponent<Camera>();
	camera->init();
    EC.addComponent(mCameraSceneObject, camera);

	camera->getGPUCamera().setPerspective(0.1, 10000, GET_SYSTEM(WindowManager).getMainWindow()->getAspectRatio(), 90);

    GET_SYSTEM(CameraManager).setCamera(camera);
}

void ScenesManager::update()
{
	PROFILER_CPU()
    
    TComponentPtr<Camera> cameraComponent = EC.getFirstComponent<Camera>(mCameraSceneObject);
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

WeakPtr<Scene> ScenesManager::getScene(HashedString sceneName) const
{
    return mScenes.at(sceneName);
}

void ScenesManager::internalLoadScene(HashedString sceneName)
{
	mLoadedScenes.insert_or_assign(sceneName, mScenes.at(sceneName));
    mLoadedScenes.at(sceneName)->loadScene();
}
