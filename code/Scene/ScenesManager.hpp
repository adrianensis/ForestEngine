#pragma once

#include "Core/Module.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"


class ScenesManager: public ObjectBase, public Singleton<ScenesManager>
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


    void addScene(OwnerPtr<Scene> newScene);

private:
	std::vector<OwnerPtr<Scene>> mScenes;
	u32 mCurrentSceneIndex = 0;

    OwnerPtr<Scene> mCurrentScene;
	bool mSceneHasChanged = false;
    OwnerPtr<GameObject> mGameObjectController;

public:
	GET(CurrentScene)
	GET(SceneHasChanged)
	GET_SET(GameObjectController)
};
