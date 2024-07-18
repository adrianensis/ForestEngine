#pragma once

#include "Core/Minimal.hpp"
#include "Core/Config/ConfigObject.hpp"
#include "Core/ECS/GameObjectsManager.hpp"

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
    void addGameObject(Ptr<GameObject> gameObject);

    template <class T> T_EXTENDS(T, GameObject)
	Ptr<T> createGameObject()
	{
        PROFILER_CPU()
        CHECK_MSG(IS_BASE_OF(GameObject, T), "T class is not derived from GameObject");
		GameObjectHandler gameObjectHandler = GET_SYSTEM(GameObjectsManager).requestGameObject<T>();
		Ptr<GameObject> gameObject = gameObjectHandler.getGameObject();
        gameObject->init();
        addGameObject(gameObject);
        return Ptr<T>::cast(gameObject);
	}
    void removeGameObject(Ptr<GameObject> gameObject);
    void update();
    void flushNewGameObjects();
    bool thereAreNewGameObjects() const;

private:
    void destroyGameObjects();

private:
    HashedString mSceneName;
	std::list<Ptr<GameObject>> mGameObjects;
	std::list<Ptr<GameObject>> mNewGameObjects;

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
