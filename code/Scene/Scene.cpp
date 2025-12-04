#include "Scene/Scene.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Scene/GameObject.hpp"
#include "Scene/Transform.hpp"
#include "Engine/EngineConfig.hpp"

void Scene::terminate()
{
    destroyGameObjects();
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

	// FOR_LIST(it, mGameObjects)
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

//	SERIALIZE_LIST_IF("objects", mGameObjects, [](OwnerEC::EntityPtrBase gameObject)
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
//			addGameObject(*it);
//		}
//	}
}

void Scene::loadScene()
{

}

void Scene::unloadScene()
{
	destroyGameObjects();
}

void Scene::addGameObject(GameObject* gameObject)
{
    if(gameObject)
    {
        gameObject->mScene = this;
        gameObject->onAddedToScene();
        mNewGameObjects.emplace_back(gameObject);
    }
}

void Scene::removeGameObject(GameObject* gameObject)
{
	if (gameObject && !gameObject->getIsDestroyed())
	{
        auto it = std::find(mGameObjects.begin(), mGameObjects.end(), gameObject);
        if (it != mGameObjects.end())
        {
            mGameObjects.erase(it);
        }

        auto itNew = std::find(mNewGameObjects.begin(), mNewGameObjects.end(), gameObject);
        if (itNew != mNewGameObjects.end())
        {
            mNewGameObjects.erase(itNew);
        }

        gameObject->destroy();
    }
}

void Scene::update()
{
	/*if (mGameObjectsToLoadIndex < mGameObjectsToLoadTotal)
	{
		FOR_RANGE_COND(i, 0, mMaxGameObjectsToLoadPerFrame, mGameObjectsToLoadIndex < mGameObjectsToLoadTotal)
		{
			std::string className = "GameObject"; //mLoadSceneConfig->at("class").get<std::string>();

			GameObject *gameObject = (GameObject*) INSTANCE_BY_NAME(className); //Core::Memory::fromClassName<GameObject>(className));
			gameObject->init();
			gameObject->deserialize(Core::JSON());
			addGameObject(gameObject);
			mGameObjectsToLoadIndex += 1;
		}
	}*/

	if (thereAreNewGameObjects())
	{
		flushNewGameObjects();
	}
}

void Scene::flushNewGameObjects()
{
	FOR_LIST(it, mNewGameObjects)
	{
		mGameObjects.emplace_back(std::move(*it));
	}

	mNewGameObjects.clear();
}

bool Scene::thereAreNewGameObjects() const
{
	return mNewGameObjects.size() > 0;
}

void Scene::destroyGameObjects()
{
	std::list<GameObject*> immutableList(mGameObjects);

	FOR_LIST(it, immutableList)
	{
		GameObject* gameObject = *it;
        if (gameObject)
		{
            if (!gameObject->getIsDestroyed())
            {
                gameObject->destroy();
            }
        }
	}

    mGameObjects.clear();
}
