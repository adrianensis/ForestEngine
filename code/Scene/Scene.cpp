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

	GameObject *cameraGameObject = Memory::newObject<GameObject>();
	cameraGameObject->init();

	cameraGameObject->mTransform.get().translate(Vector3(0, 0, 100.0f));
	cameraGameObject->mTransform.get().mScale = Vector3(1,1,1);

	Camera *cameraComponent = Memory::newObject<Camera>();
	cameraComponent->init();
	cameraComponent->setPerspective(1, 1000, RenderContext::getAspectRatio(), 45);

	cameraGameObject->addComponent<Camera>(cameraComponent);

	mCameraGameObject = cameraGameObject;

	mSize = EngineConfig::getInstance().getConfig().at("scene").at("defaultSize").get<f32>();

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
		mSize = EngineConfig::getInstance().getConfig().at("scene").at("defaultSize").get<f32>();
	}

	deserialize(mLoadSceneConfig.getJson());
}

IMPLEMENT_SERIALIZATION(Scene)
{
	f32 maxSize = 0;

	FOR_LIST(it, mGameObjects)
	{
		GameObject* obj = *it;
		if(obj)
		{
			if (obj->mShouldPersist)
			{
				Transform *t = &obj->mTransform.get();
				Vector3 worldPosition = t->getWorldPosition();
				Vector3 scale = t->mScale;

				f32 maxObjectScale = std::max(std::abs(scale.x), std::abs(scale.y));
				maxSize = std::max(std::max(maxSize, std::abs(worldPosition.x) + maxObjectScale),
								std::abs(worldPosition.y) + maxObjectScale);
			}
		}
	}

	SERIALIZE_LIST_IF("objects", mGameObjects, [](GameObject* gameObject)
	{
		return gameObject->mShouldPersist;
	})

	SERIALIZE("size", maxSize * 2.0f)
}

IMPLEMENT_DESERIALIZATION(Scene)
{ 
	DESERIALIZE("size", mSize)

	if(json.contains("objects"))
	{
		std::list<GameObject *> tmpList;
		DESERIALIZE_LIST("objects", tmpList, [](const JSON& json)
		{
			GameObject *gameObject = INSTANCE_BY_NAME(json["class"], GameObject);
			return gameObject;
		})

		FOR_LIST(it, tmpList)
		{
			(*it)->init();
			addGameObject(*it);
		}
	}
}

void Scene::unloadScene()
{
	destroyGameObjects();
}

void Scene::addGameObject(GameObject *gameObject)
{
	gameObject->mScene = (this);
	mNewGameObjects.push_back(gameObject);
}

void Scene::removeGameObject(GameObject *gameObject)
{
	if (!gameObject->getIsDestroyed() and !gameObject->getIsPendingToBeDestroyed())
	{
		mGameObjects.remove(gameObject);

		gameObject->destroy();
		gameObject->finallyDestroy();
        Memory::deleteObject(gameObject);
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
            Memory::deleteObject((*it));
		}
	}

	if (mCameraGameObject)
	{
		Ptr<Camera> cameraComponent = mCameraGameObject->getFirstComponent<Camera>();
		mCameraGameObject->removeComponent<Camera>(cameraComponent);
		mCameraGameObject->destroy();
		Memory::deleteObject(mCameraGameObject);
	}
}
