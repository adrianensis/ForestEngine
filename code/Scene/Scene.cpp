#include "Scene/Scene.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"
#include "Scene/GameObject.hpp"
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

	// FOR_LIST(it, mSceneObjects)
	// {
	// 	if((*it))
	// 	{
	// 		if ((*it)->mShouldPersist)
	// 		{
	// 			Transform *t = &(*it)->mTransform.get();
	// 			Maths::Vector3 worldPosition = t->getWorldPosition();
	// 			Maths::Vector3 scale = t->getLocalScale();

	// 			Core::f32 maxObjectScale = std::max(std::abs(scale.x), std::abs(scale.y));
	// 			maxSize = std::max(std::max(maxSize, std::abs(worldPosition.x) + maxObjectScale),
	// 							std::abs(worldPosition.y) + maxObjectScale);
	// 		}
	// 	}
	// }

//	SERIALIZE_LIST_IF("objects", mSceneObjects, [](OwnerEC::EntityPtrBase gameObject)
//	{
//		return gameObject->mShouldPersist;
//	})

	SERIALIZE("size", maxSize * 2.0f)
}

IMPLEMENT_DESERIALIZATION(Scene)
{ 
	DESERIALIZE("size", mSize)

//	if(json.contains("objects"))
//	{
//		std::list<GameObject *> tmpList;
//		DESERIALIZE_LIST("objects", tmpList, [](const Core::JSON& json)
//		{
//			GameObject *gameObject = nullptr;//INSTANCE_BY_NAME(json["class"], GameObject);
//			return gameObject;
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

void Scene::addSceneObject(EC::EntityPtr<GameObject> gameObject)
{
    if(gameObject)
    {
        gameObject->mScene = this;
        gameObject->onAddedToScene();
        mNewSceneObjects.emplace_back(gameObject);
    }
}

void Scene::removeSceneObject(EC::EntityPtr<GameObject> gameObject)
{
	if (gameObject && !gameObject->getIsDestroyed())
	{
        ECManager.destroyEntity(gameObject);

        auto it = std::find(mSceneObjects.begin(), mSceneObjects.end(), gameObject);
        if (it != mSceneObjects.end())
        {
            mSceneObjects.erase(it);
        }

        auto itNew = std::find(mNewSceneObjects.begin(), mNewSceneObjects.end(), gameObject);
        if (itNew != mNewSceneObjects.end())
        {
            mNewSceneObjects.erase(itNew);
        }

        ECManager.removeEntity(gameObject);
    }
}

void Scene::update()
{
	/*if (mSceneObjectsToLoadIndex < mSceneObjectsToLoadTotal)
	{
		FOR_RANGE_COND(i, 0, mMaxSceneObjectsToLoadPerFrame, mSceneObjectsToLoadIndex < mSceneObjectsToLoadTotal)
		{
			std::string className = "GameObject"; //mLoadSceneConfig->at("class").get<std::string>();

			GameObject *gameObject = (GameObject*) INSTANCE_BY_NAME(className); //Core::Memory::fromClassName<GameObject>(className));
			gameObject->init();
			gameObject->deserialize(Core::JSON());
			addSceneObject(gameObject);
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
	std::list<EC::EntityPtr<GameObject>> immutableList(mSceneObjects);

	FOR_LIST(it, immutableList)
	{
		EC::EntityPtr<GameObject> gameObject = *it;
        if (gameObject)
		{
            if (!gameObject->getIsDestroyed())
            {
                ECManager.destroyEntity(gameObject);
                gameObject.reset();
            }
        }
	}

    mSceneObjects.clear();
}
