#include "ScenesManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneObject.hpp"
#include "Engine/EngineConfig.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Engine/Window/WindowManager.hpp"

void ScenesManager::terminate() 
{
    if(mSceneObjectController)
    {
	    mSceneObjectController->destroy();
    }
    mSceneObjectController.reset();

	if (mCameraSceneObject)
	{
        EC::TComponentPtr<Camera> cameraComponent = ECManager.getFirstComponent<Camera>(mCameraSceneObject);
        ECManager.removeComponent(mCameraSceneObject, cameraComponent);
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
    mScenes.emplace(smDefaultSceneName, Core::OwnerPtr<Scene>::newObject());
    mScenes.emplace(smDefaultUISceneName, Core::OwnerPtr<Scene>::newObject());

    mScenes.at(smDefaultSceneName)->init(smDefaultSceneName);
    mScenes.at(smDefaultUISceneName)->init(smDefaultUISceneName);

    requestLoadScene(smDefaultSceneName);
    requestLoadScene(smDefaultUISceneName);

    mCameraSceneObject = ECManager.requestEntity<SceneObject>();
	mCameraSceneObject->init();

	// mCameraSceneObject->mTransform->setLocalPosition(Maths::Vector3(0, 0, 10));
	mCameraSceneObject->mTransform->setLocalPosition(Maths::Vector3(0, 0, 0.3f));

    EC::TComponentPtr<Camera> camera = ECManager.requestComponent<Camera>();
	camera->init();
    ECManager.addComponent(mCameraSceneObject, camera);

	camera->getGPUCamera().setPerspective(0.1, 10000, GET_SYSTEM(Window::WindowManager).getMainWindow()->getAspectRatio(), 90);

    GET_SYSTEM(CameraManager).setCamera(camera);
}

void ScenesManager::update()
{
	PROFILER_CPU()
    
    EC::TComponentPtr<Camera> cameraComponent = ECManager.getFirstComponent<Camera>(mCameraSceneObject);
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
        Core::HashedString sceneName = *it;
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

void ScenesManager::requestLoadScene(Core::HashedString sceneName)
{
	mLoadRequests.insert(sceneName);
}

Core::WeakPtr<Scene> ScenesManager::getScene(Core::HashedString sceneName) const
{
    return mScenes.at(sceneName);
}

void ScenesManager::internalLoadScene(Core::HashedString sceneName)
{
	mLoadedScenes.insert_or_assign(sceneName, mScenes.at(sceneName));
    mLoadedScenes.at(sceneName)->loadScene();
}
