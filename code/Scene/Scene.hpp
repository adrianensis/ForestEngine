#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/Config/ConfigObject.hpp"

class GameObject;

class Scene: public ObjectBase
{
    
    DECLARE_SERIALIZATION()

public:
    Scene();
    ~Scene() override;

    void init(HashedString sceneName);
    void loadScene();
    void unloadScene();
    void terminate();
    void saveToFile(const std::string& path);
    void loadToFile(const std::string& path);
    void addGameObject(OwnerPtr<GameObject>&& gameObject);

    template <class T> T_EXTENDS(T, GameObject)
	Ptr<T> createGameObject()
	{
		OwnerPtr<T> gameObject = OwnerPtr<T>::newObject();
        gameObject->init();
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
    HashedString mSceneName;
	std::list<OwnerPtr<GameObject>> mGameObjects;
	std::list<OwnerPtr<GameObject>> mNewGameObjects;

	f32 mSize = 0.0f;
	std::string mPath;

	ConfigObject mLoadSceneConfig;

public:
	CRGET(GameObjects)
	CRGET(NewGameObjects)
	GET(Size)
	GET(Path)
};
REGISTER_CLASS(Scene);
