#pragma once

#include "Engine/Core.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"

class Camera;


class ScenesManagerData
{
public:
    CameraManager* mCameraManager = nullptr;
    Core::f32 mAspectRatio = 0;
};

class ScenesManager: public System::System
{
public:	
    void init(const ScenesManagerData& scenesManagerData);
    virtual void terminate() override;
    void update();
    Core::WeakPtr<Scene> createScene(Core::HashedString sceneName);
    void loadPendingScenes();
    bool pendingLoadRequests() const;
    void requestLoadScene(Core::HashedString sceneName);
    Core::WeakPtr<Scene> getScene(Core::HashedString sceneName) const;

private:
    void internalLoadScene(Core::HashedString sceneName);

private:
	std::unordered_map<Core::HashedString, Core::OwnerPtr<Scene>> mScenes;
	std::unordered_map<Core::HashedString, Core::WeakPtr<Scene>> mLoadedScenes;
	std::unordered_set<Core::HashedString> mLoadRequests;

	GameObject* mCameraGameObject = nullptr;

public:
    inline static const Core::HashedString smGlobalSceneName = Core::HashedString("Global");
    inline static const Core::HashedString smDefaultSceneName = Core::HashedString("Default");
    inline static const Core::HashedString smDefaultUISceneName = Core::HashedString("DefaultUI");

public:
	GET(CameraGameObject)
};
REGISTER_CLASS(ScenesManager, System);
