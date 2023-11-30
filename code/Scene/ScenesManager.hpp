#pragma once

#include "Core/Module.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"

class ScenesManager: public System
{
	GENERATE_METADATA(ScenesManager)

public:
    ScenesManager();
    ~ScenesManager() override;
	
    void init();
    void update();
    void loadCurrentScene();
    void setScene(u32 i);

private:
    void internalLoadScene();

private:
	std::vector<OwnerPtr<Scene>> mScenes;
	u32 mCurrentSceneIndex = 0;

    Ptr<Scene> mCurrentScene;
	bool mSceneHasChanged = false;
    OwnerPtr<GameObject> mGameObjectController;

public:
	RGET(CurrentScene)
	GET(SceneHasChanged)
	GET(GameObjectController)
	SETMOVE(GameObjectController)
};
