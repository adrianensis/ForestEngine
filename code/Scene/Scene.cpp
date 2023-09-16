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
	destroyGameObjects();
}

void Scene::init()
{
	TRACE()

	mSize = 0;

	mPath = "config/sceneTmp.json";

	mCameraGameObject = OwnerPtr<GameObject>::newObject();
	mCameraGameObject->init();

	mCameraGameObject->mTransform->translate(Vector3(0, 0, 100.0f));

    Ptr<Camera> cameraComponent = mCameraGameObject->createComponent<Camera>();
	cameraComponent->setPerspective(0.1, 10000, GET_SYSTEM(Window).getAspectRatio(), 90);

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
				Vector3 scale = t->getScale();

				f32 maxObjectScale = std::max(std::abs(scale.x), std::abs(scale.y));
				maxSize = std::max(std::max(maxSize, std::abs(worldPosition.x) + maxObjectScale),
								std::abs(worldPosition.y) + maxObjectScale);
			}
		}
	}

//	SERIALIZE_LIST_IF("objects", mGameObjects, [](OwnerPtr<GameObject> gameObject)
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

void Scene::addGameObject(OwnerPtr<GameObject>&& gameObject)
{
	gameObject->mScene = getPtrToThis();
	mNewGameObjects.emplace_back(std::move(gameObject));
}

void Scene::removeGameObject(Ptr<GameObject> gameObject)
{
	if (!gameObject->getIsDestroyed() and !gameObject->getIsPendingToBeDestroyed())
	{
        gameObject->destroy();
        gameObject->finallyDestroy();

        std::remove_if(
        mGameObjects.begin(), mGameObjects.end(),
        [gameObject](OwnerPtr<GameObject>& go) { return go == gameObject; });
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
                (*it).invalidate();
            }
        }
	}

    mGameObjects.clear();

	if (mCameraGameObject)
	{
		Ptr<Camera> cameraComponent = mCameraGameObject->getFirstComponent<Camera>();
		mCameraGameObject->removeComponent<Camera>(cameraComponent);
		mCameraGameObject->destroy();
        mCameraGameObject.invalidate();
	}
}
