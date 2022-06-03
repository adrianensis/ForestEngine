#ifndef SCENE_HPP
#define SCENE_HPP

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
    void saveScene(const String& path);
    void loadScene(const String& path);
    void unloadScene();
    void addGameObject(GameObject *gameObject);
    void removeGameObject(GameObject *gameObject);
    void update();
    void flushNewGameObjects();
    bool thereAreNewGameObjects() const;

private:
    void destroyGameObjects();

private:
	std::list<GameObject *> mGameObjects;
	std::list<GameObject *> mNewGameObjects;
	GameObject* mCameraGameObject;

	f32 mSize = 0.0f;
	String mPath;

	ConfigObject mLoadSceneConfig;

public:
	GET(GameObjects)
	GET(NewGameObjects)
	GET(CameraGameObject)
	GET(Size)
	GET(Path)
};


#endif