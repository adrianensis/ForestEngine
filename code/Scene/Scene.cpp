#include "Scene/Scene.hpp"
#include "Scene/SceneObject.hpp"
#include "Scene/Transform.hpp"
#include "Engine/EngineConfig.hpp"

void Scene::terminate()
{
    destroySceneObjects();
}

void Scene::init(Core::HashedString sceneName)
{
    mSceneName = sceneName;
	mSize = 0;

	mPath = "config/sceneTmp.json";
	mSize = GET_SYSTEM(EngineConfig).getConfig().at("scene").at("defaultSize").get<Core::f32>();
}

void Scene::saveToFile(const std::string& path)
{
	mPath = path;

	Config::Config configMap;

	Core::JSON json;
	serialize(json);

	configMap.setJson(json);

	configMap.writeToJsonFile(path);
}

void Scene::loadToFile(const std::string& path)
{
	mPath = path;

	mLoadSceneConfig.readFromJsonFile(mPath); // PERF: do async / in other thread.

    mSize = GET_SYSTEM(EngineConfig).getConfig().at("scene").at("defaultSize").get<Core::f32>();
	if (mLoadSceneConfig.contains("size"))
	{
		mSize = mLoadSceneConfig.at("size").get<Core::f32>();
	}

	deserialize(mLoadSceneConfig.getJson());
}

IMPLEMENT_SERIALIZATION(Scene)
{
	Core::f32 maxSize = 0;

	FOR_LIST(it, mSceneObjects)
	{
		if((*it))
		{
			if ((*it)->mShouldPersist)
			{
				Transform *t = &(*it)->mTransform.get();
				Maths::Vector3 worldPosition = t->getWorldPosition();
				Maths::Vector3 scale = t->getLocalScale();

				Core::f32 maxObjectScale = std::max(std::abs(scale.x), std::abs(scale.y));
				maxSize = std::max(std::max(maxSize, std::abs(worldPosition.x) + maxObjectScale),
								std::abs(worldPosition.y) + maxObjectScale);
			}
		}
	}

//	SERIALIZE_LIST_IF("objects", mSceneObjects, [](OwnerEC::EntityPtr sceneObject)
//	{
//		return sceneObject->mShouldPersist;
//	})

	SERIALIZE("size", maxSize * 2.0f)
}

IMPLEMENT_DESERIALIZATION(Scene)
{ 
	DESERIALIZE("size", mSize)

//	if(json.contains("objects"))
//	{
//		std::list<SceneObject *> tmpList;
//		DESERIALIZE_LIST("objects", tmpList, [](const Core::JSON& json)
//		{
//			SceneObject *sceneObject = nullptr;//INSTANCE_BY_NAME(json["class"], SceneObject);
//			return sceneObject;
//		})
//
//		FOR_LIST(it, tmpList)
//		{
//			(*it)->init();
//			addSceneObject(*it);
//		}
//	}
}

void Scene::loadScene()
{

}

void Scene::unloadScene()
{
	destroySceneObjects();
}

void Scene::addSceneObject(EC::TEntityPtr<SceneObject> sceneObject)
{
    if(sceneObject)
    {
        sceneObject->mScene = getPtrToThis<Scene>();
        sceneObject->onAddedToScene();
        mNewSceneObjects.emplace_back(sceneObject);
    }
}

void Scene::removeSceneObject(EC::TEntityPtr<SceneObject> sceneObject)
{
	if (sceneObject && !sceneObject->getIsDestroyed() && !sceneObject->getIsPendingToBeDestroyed())
	{
        sceneObject->destroy();
        sceneObject->finallyDestroy();

        auto it = std::find(mSceneObjects.begin(), mSceneObjects.end(), sceneObject);
        if (it != mSceneObjects.end())
        {
            mSceneObjects.erase(it);
        }

        auto itNew = std::find(mNewSceneObjects.begin(), mNewSceneObjects.end(), sceneObject);
        if (itNew != mNewSceneObjects.end())
        {
            mNewSceneObjects.erase(itNew);
        }

        ECManager.removeEntity(sceneObject);
    }
}

void Scene::update()
{
	/*if (mSceneObjectsToLoadIndex < mSceneObjectsToLoadTotal)
	{
		FOR_RANGE_COND(i, 0, mMaxSceneObjectsToLoadPerFrame, mSceneObjectsToLoadIndex < mSceneObjectsToLoadTotal)
		{
			std::string className = "SceneObject"; //mLoadSceneConfig->at("class").get<std::string>();

			SceneObject *sceneObject = (SceneObject*) INSTANCE_BY_NAME(className); //Core::Memory::fromClassName<SceneObject>(className));
			sceneObject->init();
			sceneObject->deserialize(Core::JSON());
			addSceneObject(sceneObject);
			mSceneObjectsToLoadIndex += 1;
		}
	}*/

	if (thereAreNewSceneObjects())
	{
		flushNewSceneObjects();
	}
}

void Scene::flushNewSceneObjects()
{
	FOR_LIST(it, mNewSceneObjects)
	{
		mSceneObjects.emplace_back(std::move(*it));
	}

	mNewSceneObjects.clear();
}

bool Scene::thereAreNewSceneObjects() const
{
	return mNewSceneObjects.size() > 0;
}

void Scene::destroySceneObjects()
{
	std::list<EC::TEntityPtr<SceneObject>> immutableList(mSceneObjects);

	FOR_LIST(it, immutableList)
	{
		EC::TEntityPtr<SceneObject> sceneObject = *it;
        if (sceneObject)
		{
            if (!sceneObject->getIsDestroyed())
            {
                sceneObject->destroy();
                sceneObject.reset();
            }
        }
	}

    mSceneObjects.clear();
}
