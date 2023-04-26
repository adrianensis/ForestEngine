#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Scene/Transform.hpp"
#include "Graphics/Module.hpp"
#include "Core/EngineConfig.hpp"


Scene::Scene()
{

}

Scene::~Scene() 
{
	destroyGameObjects();
}

void Scene::init()
{
	TRACE()

	mSize = 0;

	mPath = "config/sceneTmp.json";

	mCameraGameObject = SharedPtr<GameObject>::newObject();
	mCameraGameObject->init();

	mCameraGameObject->mTransform->translate(Vector3(0, 0, 100.0f));
	mCameraGameObject->mTransform->mScale = Vector3(1,1,1);

    SharedPtr<Camera> cameraComponent = SharedPtr<Camera>::newObject();
	cameraComponent->init();
	cameraComponent->setPerspective(1, 1000, GET_SYSTEM(RenderContext).getAspectRatio(), 45);

	mCameraGameObject->addComponent<Camera>(cameraComponent);

	mSize = GET_SYSTEM(EngineConfig).getConfig().at("scene").at("defaultSize").get<f32>();

	mLoadSceneConfig.init();
}

void Scene::saveScene(const std::string& path)
{
	mPath = path;

	ConfigObject configMap;
	configMap.init();

	JSON json;
	serialize(json);

	configMap.setJson(json);

	configMap.writeToJsonFile(path);
}

void Scene::loadScene(const std::string& path)
{
	mPath = path;

	mLoadSceneConfig.clear();

	mLoadSceneConfig.readFromJsonFile(mPath); // TODO: do async / in other thread.

	mSize = mLoadSceneConfig.at("size").get<f32>();

	if (mSize == 0)
	{
		mSize = GET_SYSTEM(EngineConfig).getConfig().at("scene").at("defaultSize").get<f32>();
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
				Vector3 scale = t->mScale;

				f32 maxObjectScale = std::max(std::abs(scale.x), std::abs(scale.y));
				maxSize = std::max(std::max(maxSize, std::abs(worldPosition.x) + maxObjectScale),
								std::abs(worldPosition.y) + maxObjectScale);
			}
		}
	}

//	SERIALIZE_LIST_IF("objects", mGameObjects, [](SharedPtr<GameObject> gameObject)
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

void Scene::unloadScene()
{
	destroyGameObjects();
}

void Scene::addGameObject(SharedPtr<GameObject> gameObject)
{
	gameObject->mScene = getPtrToThis();
	mNewGameObjects.push_back(gameObject);
}

void Scene::removeGameObject(Ptr<GameObject> gameObject)
{
	if (!gameObject->getIsDestroyed() and !gameObject->getIsPendingToBeDestroyed())
	{
        gameObject->destroy();
        gameObject->finallyDestroy();

        mGameObjects.remove(gameObject);
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
		/*FOR_LIST(it, mNewGameObjects)
		{
			updateComponents(*it);
		}*/

		flushNewGameObjects();
	}
}

void Scene::flushNewGameObjects()
{
	FOR_LIST(it, mNewGameObjects)
	{
		mGameObjects.push_back(*it);
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
		if (!(*it)->getIsDestroyed())
		{
			(*it)->destroy();
            (*it).invalidate();
		}
	}

	if (mCameraGameObject)
	{
		Ptr<Camera> cameraComponent = mCameraGameObject->getFirstComponent<Camera>();
		mCameraGameObject->removeComponent<Camera>(cameraComponent);
		mCameraGameObject->destroy();
        mCameraGameObject.invalidate();
	}
}
