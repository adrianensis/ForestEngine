#pragma once

#include "Engine/Minimal.hpp"
#include "Assets/Config/ConfigObject.hpp"

class GameObject;

class Scene: public ObjectBase
{
    
    DECLARE_SERIALIZATION()

public:
    Scene();
    ~Scene() override;

    void init();
    void saveScene(const std::string& path);
    void loadScene(const std::string& path);
    void unloadScene();
    void addGameObject(OwnerPtr<GameObject>&& gameObject);

    template <class T, typename ... Args> T_EXTENDS(T, GameObject)
	Ptr<T> createGameObject(Args&&... args)
	{
		OwnerPtr<T> gameObject = OwnerPtr<T>::newObject();
        gameObject->init(args...);
		Ptr<T> gameObjectPtr = gameObject;
        addGameObject(std::move(OwnerPtr<GameObject>::moveCast(gameObject)));
        return gameObjectPtr;
	}
    void removeGameObject(Ptr<GameObject> gameObject);
    void update();
    void flushNewGameObjects();
    bool thereAreNewGameObjects() const;

private:
    void destroyGameObjects();

private:
	std::list<OwnerPtr<GameObject>> mGameObjects;
	std::list<OwnerPtr<GameObject>> mNewGameObjects;
	OwnerPtr<GameObject> mCameraGameObject;

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
REGISTER_CLASS(Scene);
