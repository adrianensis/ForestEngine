#pragma once

#include "Core/Module.hpp"


class GameObject;

class Scene: public ObjectBase
{
    GENERATE_METADATA(Scene)
    DECLARE_SERIALIZATION()

public:
    Scene();
    ~Scene() override;

    void init();
    void saveScene(const std::string& path);
    void loadScene(const std::string& path);
    void unloadScene();
    void addGameObject(SharedPtr<GameObject> gameObject);
    void removeGameObject(Ptr<GameObject> gameObject);
    void update();
    void flushNewGameObjects();
    bool thereAreNewGameObjects() const;

private:
    void destroyGameObjects();

private:
	std::list<SharedPtr<GameObject>> mGameObjects;
	std::list<SharedPtr<GameObject>> mNewGameObjects;
	SharedPtr<GameObject> mCameraGameObject;

	f32 mSize = 0.0f;
	std::string mPath;

	ConfigObject mLoadSceneConfig;

public:
	CRGET(GameObjects)
	CRGET(NewGameObjects)
	GET(CameraGameObject)
	GET(Size)
	GET(Path)
};

