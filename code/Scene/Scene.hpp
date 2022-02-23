#pragma once

#include "Core/Module.hpp"
class GameObject;

class Scene: public ObjectBase
{
    GENERATE_METADATA(Scene)
	PRI std::list<GameObject *> mGameObjects;
	PRI std::list<GameObject *> mNewGameObjects;
	PRI GameObject* mCameraGameObject;

	PRI f32 mSize = 0.0f;
	PRI std::string mPath;

	PRI ConfigObject mLoadSceneConfig;

PRI
	void destroyGameObjects();

public:
	Scene();
	~Scene() override;

	void init();

	void serialize(JSON& json) const override;
	void deserialize(const JSON& json) override;
	void loadScene(const std::string& path);
	void saveScene(const std::string& path);

	void unloadScene();

	void addGameObject(GameObject * gameObject);

	void removeGameObject(GameObject * gameObject);

	void update();

	bool thereAreNewGameObjects() const;
	void flushNewGameObjects();

	GET(GameObjects)
	GET(NewGameObjects)
	GET_SET(CameraGameObject)
	GET(Size)
	GET(Path)
};
