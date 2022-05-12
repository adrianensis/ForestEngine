#ifndef SCENESMANAGER_HPP
#define SCENESMANAGER_HPP

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


    void addScene(Scene *newScene);

private:
	std::vector<Scene *> mScenes;
	u32 mCurrentSceneIndex = 0;

	Scene* mCurrentScene = nullptr;
	bool mSceneHasChanged = false;
	GameObject* mGameObjectController = nullptr;

public:
	GET(CurrentScene)
	GET(SceneHasChanged)
	GET_SET(GameObjectController)
};

#endif