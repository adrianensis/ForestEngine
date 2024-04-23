#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"

class Camera;

class ScenesManager: public System
{
public:
    ScenesManager();
    ~ScenesManager() override;
	
    virtual void init() override;
    virtual void terminate() override;
    void update();
    void loadCurrentScene();
    void setScene(u32 i);

private:
    void internalLoadScene();

private:
	std::vector<OwnerPtr<Scene>> mScenes;
	u32 mCurrentSceneIndex = 0;
    Ptr<Camera> mCurrentCamera;

    Ptr<Scene> mCurrentScene;
	bool mSceneHasChanged = false;
    OwnerPtr<GameObject> mGameObjectController;

public:
	GET(CurrentCamera)
	RGET(CurrentScene)
	GET(SceneHasChanged)
	GET(GameObjectController)
	SETMOVE(GameObjectController)
};
REGISTER_CLASS(ScenesManager);
