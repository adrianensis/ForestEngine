#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "Scene/Scene.hpp"
#include "Scene/GameObject.hpp"
#include "Scene/Transform.hpp"
#include "Graphics/Module.hpp"
#include "Core/EngineConfig.hpp"
#endif

class GameObject;

class Scene: public ObjectBase
{
    GENERATE_METADATA(Scene)

public:
	CPP Scene()
	{

	}

	CPP ~Scene() override
	{
		destroyGameObjects();
	}


	CPP void init()
	{
		TRACE()

		mSize = 0;

		mPath = "config/sceneTmp.json";

		// CAMERA
		GameObject *cameraGameObject = NEW(GameObject);
		cameraGameObject->init();

		cameraGameObject->getTransform().get().translate(Vector3(0, 0, 100.0f));
		cameraGameObject->getTransform().get().setScale(Vector3(1,1,1));
		//cameraGameObject->getTransform().get().setLocalPosition(Vector3(0, 0, 0));

		Camera *cameraComponent = NEW(Camera);
		cameraComponent->init();

		f32 size = RenderContext::getWindowSize().y;
		// TODO : use RenderContext::getWindowSize().x also? To keep the scaleproportions?
		//cameraComponent->setOrtho(-size, size, -size, size, 1000, -1000);
		cameraComponent->setPerspective(1, 1000, RenderContext::getAspectRatio(), 60);

		cameraGameObject->addComponent<Camera>(cameraComponent);

		setCameraGameObject(cameraGameObject);

		// SET DEFAULT SIZE
		mSize = EngineConfig::getInstance().getConfig().at("scene").at("defaultSize").get<f32>();

		mLoadSceneConfig.init();
	}

	CPP void saveScene(const std::string& path)
	{
		mPath = path;

		ConfigObject configMap;
		configMap.init();

		JSON json;
		serialize(json);

		configMap.setJson(json);

		configMap.writeToJsonFile(path);
	}

	CPP void loadScene(const std::string& path)
	{
		mPath = path;

		mLoadSceneConfig.clear();

		//std::future<void> fut = std::async (&ConfigObject::readFromJsonFile,&mLoadSceneConfig,mPath);
		mLoadSceneConfig.readFromJsonFile(mPath); // TODO: do async / in other thread.
		//fut.wait();

		mSize = mLoadSceneConfig.at("size").get<f32>();

		if (mSize == 0)
		{
			mSize = EngineConfig::getInstance().getConfig().at("scene").at("defaultSize").get<f32>();
		}

		deserialize(mLoadSceneConfig.getJson());
	}

	CPP void serialize(JSON& json) const override
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

		DO_SERIALIZE_LIST_IF("objects", mGameObjects, [](GameObject* gameObject)
		{
			return gameObject->getShouldPersist();
		})

		DO_SERIALIZE("size", maxSize * 2.0f)
	}

	CPP void deserialize(const JSON& json) override
	{ 
		DO_DESERIALIZE("size", mSize)

		if(json.contains("objects"))
		{
			std::list<GameObject *> tmpList;
			DO_DESERIALIZE_LIST("objects", tmpList, [](const JSON& json)
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

	CPP void unloadScene()
	{
		destroyGameObjects();
	}

	CPP void addGameObject(GameObject *gameObject)
	{
		gameObject->setScene(this);
		mNewGameObjects.push_back(gameObject);
	}

	CPP void removeGameObject(GameObject *gameObject)
	{
		if (!gameObject->getIsDestroyed() && !gameObject->getIsPendingToBeDestroyed())
		{
			mGameObjects.remove(gameObject);

			gameObject->destroy();
			gameObject->finallyDestroy();
			DELETE(gameObject);
		}
	}

	CPP void update()
	{
		// TODO : refactor into a private method
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

	CPP void flushNewGameObjects()
	{
		FOR_LIST(it, mNewGameObjects)
		{
			mGameObjects.push_back(*it);
		}

		mNewGameObjects.clear();
	}

	CPP bool thereAreNewGameObjects() const
	{
		return mNewGameObjects.size() > 0;
	}

private:
	CPP void destroyGameObjects()
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
			//DELETE(cameraComponent);
			mCameraGameObject->destroy();
			DELETE(mCameraGameObject);
		}
	}

private:
	std::list<GameObject *> mGameObjects;
	std::list<GameObject *> mNewGameObjects;
	GameObject* mCameraGameObject;

	f32 mSize = 0.0f;
	std::string mPath;

	ConfigObject mLoadSceneConfig;

public:
	GET(GameObjects)
	GET(NewGameObjects)
	GET_SET(CameraGameObject)
	GET(Size)
	GET(Path)
};
