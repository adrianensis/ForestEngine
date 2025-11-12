#pragma once

#include "Engine/Core.hpp"
#include "Core/Config/Config.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Scene/GameObject.hpp"

class Scene: public Core::ISerializable
{
    DECLARE_SERIALIZATION()

public:
    void init(Core::HashedString sceneName);
    void loadScene();
    void unloadScene();
    void terminate();
    void saveToFile(const std::string& path);
    void loadToFile(const std::string& path);
    void addGameObject(EC::EntityPtr<GameObject> gameObject);

    template <class T> T_EXTENDS(T, GameObject)
	EC::EntityPtr<T> createGameObject()
	{
        PROFILER_CPU()
        CHECK_MSG(IS_BASE_OF(GameObject, T), "T class is not derived from GameObject");
		EC::EntityPtr<GameObject> entityPtr = ECManager.requestEntity<T>();
        entityPtr->init();
        addGameObject(entityPtr);
        return entityPtr;
	}
    void removeGameObject(EC::EntityPtr<GameObject> gameObject);
    void update();
    void flushNewGameObjects();
    bool thereAreNewGameObjects() const;

private:
    void destroyGameObjects();

private:
    Core::HashedString mSceneName;
	std::list<EC::EntityPtr<GameObject>> mGameObjects;
	std::list<EC::EntityPtr<GameObject>> mNewGameObjects;

	Core::f32 mSize = 0.0f;
	std::string mPath;

	Config::Config mLoadSceneConfig;

public:
	CRGET(GameObjects)
	CRGET(NewGameObjects)
	GET(Size)
	GET(Path)
};
REGISTER_CLASS(Scene);
