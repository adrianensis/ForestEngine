#pragma once

#include "Core/System/SystemsManager.hpp"
#include "Engine/Core.hpp"
#include "Core/Config/Config.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Scene/GameObject.hpp"

class ScenesManager;
class Scene: public Core::ISerializable
{
    DECLARE_SERIALIZATION()

public:
    void init(Core::HashedString sceneName, ScenesManager* scenesManager);
    void loadScene();
    void unloadScene();
    void terminate();
    void saveToFile(const std::string& path);
    void loadToFile(const std::string& path);

    template <class T> T_EXTENDS(T, GameObject)
	T* createGameObject()
	{
        PROFILER_CPU()
        CHECK_MSG(IS_BASE_OF(GameObject, T), "T class is not derived from GameObject");
		T* entityPtr = GET_SYSTEM(EC::EntityComponentManager).requestEntity<T>([&](T* entity)
        {
            entity->init(); 
        });
        
        addGameObject(entityPtr);
        return entityPtr;
	}
    void removeGameObject(GameObject* gameObject);
    void update();
    void flushNewGameObjects();
    bool thereAreNewGameObjects() const;

private:
    void addGameObject(GameObject* gameObject);
    void destroyGameObjects();

private:
    ScenesManager* mScenesManager = nullptr;
    Core::HashedString mSceneName;
	std::list<GameObject*> mGameObjects;
	std::list<GameObject*> mNewGameObjects;

	Core::f32 mSize = 0.0f;
	std::string mPath;

	Config::Config mLoadSceneConfig;

public:
	GET(ScenesManager)
	CRGET(GameObjects)
	CRGET(NewGameObjects)
	GET(Size)
	GET(SceneName)
	GET(Path)
};
REGISTER_CLASS(Scene);
