#pragma once

#include "Engine/Core.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneObject.hpp"

class Camera;

class ScenesManager: public System::System
{
public:	
    virtual void init() override;
    virtual void terminate() override;
    void update();
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

    EC::EntityPtr<SceneObject> mSceneObjectController;
	EC::EntityPtr<SceneObject> mCameraSceneObject;

public:
    inline static const Core::HashedString smDefaultSceneName = Core::HashedString("Default");
    inline static const Core::HashedString smDefaultUISceneName = Core::HashedString("DefaultUI");

public:
	GET(SceneObjectController)
	SET(SceneObjectController)
	GET(CameraSceneObject)
};
REGISTER_CLASS(ScenesManager);
