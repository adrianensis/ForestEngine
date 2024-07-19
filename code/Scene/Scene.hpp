#pragma once

#include "Core/Minimal.hpp"
#include "Core/Config/ConfigObject.hpp"
#include "Core/ECS/EntityManager.hpp"
#include "Scene/GameObject.hpp"

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
    void addGameObject(TypedEntityHandler<GameObject> gameObject);

    template <class T> T_EXTENDS(T, GameObject)
	TypedEntityHandler<T> createGameObject()
	{
        PROFILER_CPU()
        CHECK_MSG(IS_BASE_OF(GameObject, T), "T class is not derived from GameObject");
		EntityHandler entityHandler = GET_SYSTEM(EntityManager).requestEntity<T>();
        entityHandler->init();
        addGameObject(entityHandler);
        return entityHandler;
	}
    void removeGameObject(TypedEntityHandler<GameObject> gameObject);
    void update();
    void flushNewGameObjects();
    bool thereAreNewGameObjects() const;

private:
    void destroyGameObjects();

private:
    HashedString mSceneName;
	std::list<TypedEntityHandler<GameObject>> mGameObjects;
	std::list<TypedEntityHandler<GameObject>> mNewGameObjects;

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
