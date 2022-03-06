#pragma once

// CPP_IGNORE

#include "Core/Module.hpp"
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"

class ScenesManager: public ObjectBase, public Singleton<ScenesManager>
{
	GENERATE_METADATA(ScenesManager)

private:
	std::vector<Scene *> mScenes;
	u32 mCurrentSceneIndex = 0;

	Scene* mCurrentScene = nullptr;
	bool mSceneHasChanged = false;
	GameObject* mGameObjectController = nullptr;

	void internalLoadScene();
	void addScene(Scene * newScene);

public:
	ScenesManager();
	~ScenesManager() override;

	void init();
	void update();
	void loadCurrentScene();
	void setScene(u32 i);

	GET(CurrentScene)
	GET(SceneHasChanged)
	GET_SET(GameObjectController)
};