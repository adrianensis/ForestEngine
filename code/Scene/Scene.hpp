#pragma once

#include "Engine/Core.hpp"
#include "Core/Config/Config.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"
#include "Scene/SceneObject.hpp"

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
    void addSceneObject(EC::EntityPtr<SceneObject> sceneObject);

    template <class T> T_EXTENDS(T, SceneObject)
	EC::EntityPtr<T> createSceneObject()
	{
        PROFILER_CPU()
        CHECK_MSG(IS_BASE_OF(SceneObject, T), "T class is not derived from SceneObject");
		EC::EntityPtr<SceneObject> entityPtr = ECManager.requestEntity<T>();
        entityPtr->init();
        addSceneObject(entityPtr);
        return entityPtr;
	}
    void removeSceneObject(EC::EntityPtr<SceneObject> sceneObject);
    void update();
    void flushNewSceneObjects();
    bool thereAreNewSceneObjects() const;

private:
    void destroySceneObjects();

private:
    Core::HashedString mSceneName;
	std::list<EC::EntityPtr<SceneObject>> mSceneObjects;
	std::list<EC::EntityPtr<SceneObject>> mNewSceneObjects;

	Core::f32 mSize = 0.0f;
	std::string mPath;

	Config::Config mLoadSceneConfig;

public:
	CRGET(SceneObjects)
	CRGET(NewSceneObjects)
	GET(Size)
	GET(Path)
};
REGISTER_CLASS(Scene);
