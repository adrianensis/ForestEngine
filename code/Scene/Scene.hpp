#pragma once

#include "Core/Module.hpp"
class GameObject;

class Scene: public ObjectBase
{
    GENERATE_METADATA(Scene)
	PRI std::list<GameObject *> mGameObjects; GET(GameObjects);
	PRI std::list<GameObject *> mNewGameObjects; GET(NewGameObjects);
	PRI GameObject* mCameraGameObject; GET_SET(CameraGameObject)

	PRI f32 mSize = 0.0f; GET(Size)
	PRI std::string mPath; GET(Path)

	PRI ConfigObject mLoadSceneConfig;

PRI
	void destroyGameObjects();

PUB
	Scene();
	~Scene() override;

	void init();

	virtual void serialize(JSON& json) const override;
	virtual void deserialize(const JSON& json) override;
	void loadScene(const std::string& path);
	void saveScene(const std::string& path);

	void unloadScene();

	void addGameObject(GameObject * gameObject);

	void removeGameObject(GameObject * gameObject);

	void update();

	bool thereAreNewGameObjects() const;
	void flushNewGameObjects();
};
