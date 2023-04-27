#pragma once

#include "Core/Module.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"


class ScenesManager: public EngineSystem
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
	std::vector<SharedPtr<Scene>> mScenes;
	u32 mCurrentSceneIndex = 0;

    Ptr<Scene> mCurrentScene;
	bool mSceneHasChanged = false;
    SharedPtr<GameObject> mGameObjectController;

public:
	RGET(CurrentScene)
	GET(SceneHasChanged)
	GET_SET(GameObjectController)
};
