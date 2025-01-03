#pragma once

#include "Core/Minimal.hpp"
#include "Core/Config/ConfigObject.hpp"
#include "Core/ECS/EntityManager.hpp"
#include "Scene/GameObject.hpp"

class Scene: public ISerializable, public EnablePtrToThis
{
    DECLARE_SERIALIZATION()

public:
    void init(HashedString sceneName);
    void loadScene();
    void unloadScene();
    void terminate();
    void saveToFile(const std::string& path);
    void loadToFile(const std::string& path);
    void addGameObject(TEntityHandler<GameObject> gameObject);

    template <class T> T_EXTENDS(T, GameObject)
	TEntityHandler<T> createGameObject()
	{
        PROFILER_CPU()
        CHECK_MSG(IS_BASE_OF(GameObject, T), "T class is not derived from GameObject");
		EntityHandler entityHandler = EntityManager::getInstance().requestEntity<T>();
        entityHandler->init();
        addGameObject(entityHandler);
        return entityHandler;
	}
    void removeGameObject(TEntityHandler<GameObject> gameObject);
    void update();
    void flushNewGameObjects();
    bool thereAreNewGameObjects() const;

private:
    void destroyGameObjects();

private:
    HashedString mSceneName;
	std::list<TEntityHandler<GameObject>> mGameObjects;
	std::list<TEntityHandler<GameObject>> mNewGameObjects;

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
