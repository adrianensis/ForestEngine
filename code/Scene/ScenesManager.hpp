#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/System.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"

class Camera;

class ScenesManager: public System
{
public:
    ~ScenesManager() override;
	
    virtual void init() override;
    virtual void terminate() override;
    virtual void update() override;
    void loadPendingScenes();
    bool pendingLoadRequests() const;
    void requestLoadScene(HashedString sceneName);
    Ptr<Scene> getScene(HashedString sceneName) const;

private:
    void internalLoadScene(HashedString sceneName);

private:
	std::unordered_map<HashedString, OwnerPtr<Scene>> mScenes;
	std::unordered_map<HashedString, Ptr<Scene>> mLoadedScenes;
	std::unordered_set<HashedString> mLoadRequests;

    EntityHandler mGameObjectController;
	TypedEntityHandler<GameObject> mCameraGameObject;

public:
    inline static const HashedString smDefaultSceneName = HashedString("Default");
    inline static const HashedString smDefaultUISceneName = HashedString("DefaultUI");

public:
	GET(GameObjectController)
	SET(GameObjectController)
	GET(CameraGameObject)
};
REGISTER_CLASS(ScenesManager);
