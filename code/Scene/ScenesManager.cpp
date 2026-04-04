#include "ScenesManager.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Core/System/SystemsManager.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Engine/EngineConfig.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Window/WindowManager.hpp"

void ScenesManager::terminate() 
{
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

    mCameraGameObject = getScene(smGlobalSceneName)->createGameObject<GameObject>();

    Camera* camera = GET_SYSTEM(EC::EntityComponentManager).requestComponent<Camera>(mCameraGameObject, [&](auto* component)
    {
        component->init();
    });

    mCameraGameObject->mTransform->setLocalPosition(Maths::Vector3(0, 0, 0.3f));

	camera->getGPUCamera().setPerspective(0.1, 10000, GET_SYSTEM(Window::WindowManager).getMainWindow()->getAspectRatio(), 90);

    GET_SYSTEM(CameraManager).setCamera(camera);
}

void ScenesManager::update()
{
	PROFILER_CPU()

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
