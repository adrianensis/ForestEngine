#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneObject.hpp"

class Camera;

class ScenesManager: public Core::System
{
public:	
    virtual void init() override;
    virtual void terminate() override;
    void update();
    void loadPendingScenes();
    bool pendingLoadRequests() const;
    void requestLoadScene(Core::HashedString sceneName);
    WeakPtr<Scene> getScene(Core::HashedString sceneName) const;

private:
    void internalLoadScene(Core::HashedString sceneName);

private:
	std::unordered_map<Core::HashedString, OwnerPtr<Scene>> mScenes;
	std::unordered_map<Core::HashedString, WeakPtr<Scene>> mLoadedScenes;
	std::unordered_set<Core::HashedString> mLoadRequests;

    EntityPtr mSceneObjectController;
	TEntityPtr<SceneObject> mCameraSceneObject;

public:
    inline static const Core::HashedString smDefaultSceneName = Core::HashedString("Default");
    inline static const Core::HashedString smDefaultUISceneName = Core::HashedString("DefaultUI");

public:
	GET(SceneObjectController)
	SET(SceneObjectController)
	GET(CameraSceneObject)
};
REGISTER_CLASS(ScenesManager);
