#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneObject.hpp"

class Camera;

class ScenesManager: public System
{
public:	
    virtual void init() override;
    virtual void terminate() override;
    void update();
    void loadPendingScenes();
    bool pendingLoadRequests() const;
    void requestLoadScene(HashedString sceneName);
    WeakPtr<Scene> getScene(HashedString sceneName) const;

private:
    void internalLoadScene(HashedString sceneName);

private:
	std::unordered_map<HashedString, OwnerPtr<Scene>> mScenes;
	std::unordered_map<HashedString, WeakPtr<Scene>> mLoadedScenes;
	std::unordered_set<HashedString> mLoadRequests;

    EntityHandler mSceneObjectController;
	TEntityHandler<SceneObject> mCameraSceneObject;

public:
    inline static const HashedString smDefaultSceneName = HashedString("Default");
    inline static const HashedString smDefaultUISceneName = HashedString("DefaultUI");

public:
	GET(SceneObjectController)
	SET(SceneObjectController)
	GET(CameraSceneObject)
};
REGISTER_CLASS(ScenesManager);
