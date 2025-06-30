#pragma once

#include "Core/Minimal.hpp"
#include "Core/Config/Config.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Scene/SceneObject.hpp"

class Scene: public ISerializable, public Core::EnableWeakPtrToThis
{
    DECLARE_SERIALIZATION()

public:
    void init(Core::HashedString sceneName);
    void loadScene();
    void unloadScene();
    void terminate();
    void saveToFile(const std::string& path);
    void loadToFile(const std::string& path);
    void addSceneObject(TEntityPtr<SceneObject> sceneObject);

    template <class T> T_EXTENDS(T, SceneObject)
	TEntityPtr<T> createSceneObject()
	{
        PROFILER_CPU()
        CHECK_MSG(IS_BASE_OF(SceneObject, T), "T class is not derived from SceneObject");
		EntityPtr entityPtr = EC.requestEntity<T>();
        entityPtr->init();
        addSceneObject(entityPtr);
        return entityPtr;
	}
    void removeSceneObject(TEntityPtr<SceneObject> sceneObject);
    void update();
    void flushNewSceneObjects();
    bool thereAreNewSceneObjects() const;

private:
    void destroySceneObjects();

private:
    Core::HashedString mSceneName;
	std::list<TEntityPtr<SceneObject>> mSceneObjects;
	std::list<TEntityPtr<SceneObject>> mNewSceneObjects;

	f32 mSize = 0.0f;
	std::string mPath;

	Core::Config mLoadSceneConfig;

public:
	CRGET(SceneObjects)
	CRGET(NewSceneObjects)
	GET(Size)
	GET(Path)
};
REGISTER_CLASS(Scene);
