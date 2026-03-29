#include "ScenesManager.hpp"
#include "Core/Assert/Assert.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Engine/EngineConfig.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Window/WindowManager.hpp"

void ScenesManager::terminate() 
{
	if (mCameraGameObject)
	{
        mCameraGameObject->destroy();
	}

    if(!mScenes.empty())
    {
        FOR_MAP(it, mScenes)
        {
            // skip global scene, must be terminated last
            if(it->second->getSceneName() != smGlobalSceneName)
            {
                it->second->terminate();
            }
        }
    
        mScenes.at(smGlobalSceneName)->terminate();
    }

    mScenes.clear();
}

void ScenesManager::init()
{
    createScene(smGlobalSceneName);
    createScene(smDefaultSceneName);
    createScene(smDefaultUISceneName);

    requestLoadScene(smGlobalSceneName);
    requestLoadScene(smDefaultSceneName);
    requestLoadScene(smDefaultUISceneName);

    mCameraGameObject = ECManager.requestEntity<GameObject>([&](GameObject* entity)
    {
        entity->init(); 
    });

    Camera* camera = ECManager.requestComponent<Camera>(mCameraGameObject, [&](auto* component)
    {
        component->init();
    });

    mCameraGameObject->mTransform->setLocalPosition(Maths::Vector3(0, 0, 0.3f));

	camera->getGPUCamera().setPerspective(0.1, 10000, mSystemsDI.getSystem<Window::WindowManager>()->getMainWindow()->getAspectRatio(), 90);

    mSystemsDI.getSystem<CameraManager>()->setCamera(camera);
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

Core::WeakPtr<Scene> ScenesManager::createScene(Core::HashedString sceneName)
{
    if(mScenes.contains(sceneName))
    {
        CHECK_MSG(false, "{} already exists!", sceneName.get());
        return mScenes.at(sceneName);
    }

    mScenes.insert_or_assign(sceneName, Core::OwnerPtr<Scene>::newObject());
    Core::WeakPtr<Scene> scene = mScenes.at(sceneName);
    scene->init(sceneName, this);
    return scene;
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
