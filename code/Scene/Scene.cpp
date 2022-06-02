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

	GameObject *cameraGameObject = NEW(GameObject);
	cameraGameObject->init();

	cameraGameObject->getTransform().get().translate(Vector3(0, 0, 100.0f));
	cameraGameObject->getTransform().get().setScale(Vector3(1,1,1));

	Camera *cameraComponent = NEW(Camera);
	cameraComponent->init();

	f32 size = RenderContext::getWindowSize().y;
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

void Scene::serialize(JSON& json) const 
{
	f32 maxSize = 0;

	FOR_LIST(it, mGameObjects)
	{
		GameObject* obj = *it;
		if(obj)
		{
			if (obj->getShouldPersist())
			{
				Transform *t = &obj->getTransform().get();
				Vector3 worldPosition = t->getWorldPosition();
				Vector3 scale = t->getScale();

				f32 maxObjectScale = std::max(std::abs(scale.x), std::abs(scale.y));
				maxSize = std::max(std::max(maxSize, std::abs(worldPosition.x) + maxObjectScale),
								std::abs(worldPosition.y) + maxObjectScale);
			}
		}
	}

	SERIALIZE_LIST_IF("objects", mGameObjects, [](GameObject* gameObject)
	{
		return gameObject->getShouldPersist();
	})

	SERIALIZE("size", maxSize * 2.0f)
}

void Scene::deserialize(const JSON& json) 
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
	gameObject->setScene(this);
	mNewGameObjects.push_back(gameObject);
}

void Scene::removeGameObject(GameObject *gameObject)
{
	if (!gameObject->getIsDestroyed() && !gameObject->getIsPendingToBeDestroyed())
	{
		mGameObjects.remove(gameObject);

		gameObject->destroy();
		gameObject->finallyDestroy();
		DELETE(gameObject);
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
			DELETE((*it));
		}
	}

	if (mCameraGameObject)
	{
		Ptr<Camera> cameraComponent = mCameraGameObject->getFirstComponent<Camera>();
		mCameraGameObject->removeComponent<Camera>(cameraComponent);
		mCameraGameObject->destroy();
		DELETE(mCameraGameObject);
	}
}
