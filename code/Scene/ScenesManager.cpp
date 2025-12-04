#include "ScenesManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Engine/EngineConfig.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Window/WindowManager.hpp"

void ScenesManager::terminate() 
{
    if(mGameObjectController)
    {
        mGameObjectController->destroy();
    }

	if (mCameraGameObject)
	{
        Camera* cameraComponent = ECManager.getFirstComponent<Camera>(mCameraGameObject);
        mCameraGameObject->destroy();
	}

    FOR_MAP(it, mScenes)
    {
        it->second->terminate();
    }

    mScenes.clear();
}

void ScenesManager::init()
{
    Core::OwnerPtr<Scene> defaultSceneOwner = Core::OwnerPtr<Scene>::newObject();
    Core::OwnerPtr<Scene> defaultUISceneOwner = Core::OwnerPtr<Scene>::newObject();
        
    mScenes.insert_or_assign(smDefaultSceneName, Core::OwnerPtr<Scene>::moveCast(defaultSceneOwner));
    mScenes.insert_or_assign(smDefaultUISceneName, Core::OwnerPtr<Scene>::moveCast(defaultUISceneOwner));

    Core::WeakPtr<Scene> defaultScene = mScenes.at(smDefaultSceneName);
    Core::WeakPtr<Scene> defaultUIScene = mScenes.at(smDefaultUISceneName);
    defaultScene->init(smDefaultSceneName);
    defaultUIScene->init(smDefaultUISceneName);

    requestLoadScene(smDefaultSceneName);
    requestLoadScene(smDefaultUISceneName);

    mCameraGameObject = ECManager.requestEntity<GameObject>();
	mCameraGameObject->init();

	// mCameraGameObject->mTransform->setLocalPosition(Maths::Vector3(0, 0, 10));
	mCameraGameObject->mTransform->setLocalPosition(Maths::Vector3(0, 0, 0.3f));

    Camera* camera = ECManager.requestComponent<Camera>();
	camera->init();
    ECManager.addComponent(mCameraGameObject, camera);

	camera->getGPUCamera().setPerspective(0.1, 10000, GET_SYSTEM(Window::WindowManager).getMainWindow()->getAspectRatio(), 90);

    GET_SYSTEM(CameraManager).setCamera(camera);
}

void ScenesManager::update()
{
	PROFILER_CPU()
    
    Camera* cameraComponent = ECManager.getFirstComponent<Camera>(mCameraGameObject);
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
