#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Scene/Transform.hpp"
#include "Graphics/Module.hpp"
#include "Engine/EngineConfig.hpp"


Scene::Scene()
{

}

Scene::~Scene() 
{
	
}

void Scene::terminate()
{
    destroyGameObjects();
}

void Scene::init(HashedString sceneName)
{
	LOG_TRACE()

    mSceneName = sceneName;
	mSize = 0;

	mPath = "config/sceneTmp.json";
	mSize = GET_SYSTEM(EngineConfig).getConfig().at("scene").at("defaultSize").get<f32>();
}

void Scene::saveToFile(const std::string& path)
{
	mPath = path;

	ConfigObject configMap;

	JSON json;
	serialize(json);

	configMap.setJson(json);

	configMap.writeToJsonFile(path);
}

void Scene::loadToFile(const std::string& path)
{
	mPath = path;

	mLoadSceneConfig.readFromJsonFile(mPath); // TODO: do async / in other thread.

    mSize = GET_SYSTEM(EngineConfig).getConfig().at("scene").at("defaultSize").get<f32>();
	if (mLoadSceneConfig.contains("size"))
	{
		mSize = mLoadSceneConfig.at("size").get<f32>();
	}

	deserialize(mLoadSceneConfig.getJson());
}

IMPLEMENT_SERIALIZATION(Scene)
{
	f32 maxSize = 0;

	FOR_LIST(it, mGameObjects)
	{
		if((*it))
		{
			if ((*it)->mShouldPersist)
			{
				Transform *t = &(*it)->mTransform.get();
				Vector3 worldPosition = t->getWorldPosition();
				Vector3 scale = t->getLocalScale();

				f32 maxObjectScale = std::max(std::abs(scale.x), std::abs(scale.y));
				maxSize = std::max(std::max(maxSize, std::abs(worldPosition.x) + maxObjectScale),
								std::abs(worldPosition.y) + maxObjectScale);
			}
		}
	}

//	SERIALIZE_LIST_IF("objects", mGameObjects, [](OwnerEntityHandler gameObject)
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
//		DESERIALIZE_LIST("objects", tmpList, [](const JSON& json)
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

void Scene::addGameObject(TypedEntityHandler<GameObject> gameObject)
{
    if(gameObject)
    {
        gameObject->mScene = getPtrToThis<Scene>();
        gameObject->onAddedToScene();
        mNewGameObjects.emplace_back(gameObject);
    }
}

void Scene::removeGameObject(TypedEntityHandler<GameObject> gameObject)
{
	if (gameObject && !gameObject->getIsDestroyed() && !gameObject->getIsPendingToBeDestroyed())
	{
        gameObject->destroy();
        gameObject->finallyDestroy();

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

        GET_SYSTEM(EntityManager).removeEntity(gameObject);
    }
}

void Scene::update()
{
	/*if (mGameObjectsToLoadIndex < mGameObjectsToLoadTotal)
	{
		FOR_RANGE_COND(i, 0, mMaxGameObjectsToLoadPerFrame, mGameObjectsToLoadIndex < mGameObjectsToLoadTotal)
		{
			std::string className = "GameObject"; //mLoadSceneConfig->at("class").get<std::string>();

			GameObject *gameObject = (GameObject*) INSTANCE_BY_NAME(className); //Memory::fromClassName<GameObject>(className));
			gameObject->init();
			gameObject->deserialize(JSON());
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
	FOR_LIST(it, mGameObjects)
	{
        if ((*it))
		{
            if (!(*it)->getIsDestroyed())
            {
                (*it)->destroy();
                (*it).reset();
            }
        }
	}

    mGameObjects.clear();
}
