#include "Scene.h"
#include "Memory.h"
#include "List.h"
#include "GameObject.h"
#include "Transform.h"
#include "Renderer.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Vector2.h"
#include "Vector3.h"
#include "ConfigMap.h"
#include "Settings.h"
#include "Camera.h"
#include "Script.h"
#include "RigidBody.h"
#include "MaterialManager.h"
#include "MathUtils.h"
#include "ScriptEngine.h"
#include "RenderEngine.h"
#include "RenderContext.h"
#include "PhysicsEngine.h"

namespace DE {

// ---------------------------------------------------------------------------

Scene::Scene() : DE_Class(){
	mGameObjects = nullptr;
}

Scene::~Scene(){

	destroyGameObjects();

	Memory::free<List<GameObject*>>(mGameObjects);
	Memory::free<List<GameObject*>>(mNewGameObjects);
}

// ---------------------------------------------------------------------------

void Scene::destroyGameObjects(){
	FOR_LIST (it, mGameObjects){
		if(!it.get()->isDestroyed()){
			it.get()->destroy();
			Memory::free<GameObject>(it.get());
		}
	}
}
// ---------------------------------------------------------------------------

void Scene::init(){
	TRACE();

	mGameObjects = Memory::allocate<List<GameObject*>>();
	mGameObjects->init();

	mNewGameObjects = Memory::allocate<List<GameObject*>>();
	mNewGameObjects->init();

	mSize = 0;

	mPath="config/sceneTmp.conf";

	// CAMERA
	GameObject* cameraGameObject = Memory::allocate<GameObject>();
	cameraGameObject->init();

	cameraGameObject->getTransform()->setLocalPosition(Vector3(0,0,0));

	Camera* cameraComponent = Memory::allocate<Camera>();
	cameraGameObject->addComponent<Camera>(cameraComponent);

	/*
	if (aspect >= 1.0)
		ortho(-50.0 * aspect, 50.0 * aspect, -50.0, 50.0, 1.0, -1.0);
	else
		ortho(-50.0, 50.0, -50.0 / aspect, 50.0 / aspect, 1.0, -1.0);
	*/
	// f32 aspect = RenderContext::getAspectRatio();
	cameraComponent->setOrtho(-720, 720, -720, 720, 1, -1);

	setCameraGameObject(cameraGameObject);
}

// ---------------------------------------------------------------------------

void Scene::loadScene(const std::string& path){

	mPath = path; // TODO: copy?

	ConfigMap* configMap = Memory::allocate<ConfigMap>();
	configMap->init();

	configMap->readConfigFile(path);

	mSize = configMap->getF32("scene.size");

	u32 length = configMap->getU32("objects.length");

  Material* material = MaterialManager::getInstance()->loadMaterial("resources/tiles.png");

	FOR_RANGE(i, 0, length){
		std::string indexStr = std::to_string(i);
		std::string objectStr = "objects["+indexStr+"]";

		Vector2 worldPosition(configMap->getF32(objectStr+".worldPosition.x"), configMap->getF32(objectStr+".worldPosition.y"));

		Vector2 size(configMap->getF32(objectStr+".size.width"), configMap->getF32(objectStr+".size.height"));
		Vector2 textureRegionPosition(configMap->getF32(objectStr+".texture.region.u"), configMap->getF32(objectStr+".texture.region.v"));
		Vector2 textureRegionSize(configMap->getF32(objectStr+".texture.region.width"), configMap->getF32(objectStr+".texture.region.height"));

    GameObject* gameObject = Memory::allocate<GameObject>();
    gameObject->init();

    gameObject->getTransform()->setLocalPosition(Vector3(worldPosition.x,worldPosition.y,0));
    gameObject->getTransform()->setScale(Vector3(size.x,size.y,1));

    Renderer* renderer = Memory::allocate<Renderer>();
    gameObject->addComponent<Renderer>(renderer);

		renderer->setLayer(configMap->getU32(objectStr+".layer"));

		renderer->setMesh(Mesh::getRectangle());
    renderer->setMaterial(material);

		renderer->setRegion(textureRegionPosition.x, textureRegionPosition.y, textureRegionSize.x, textureRegionSize.y);

    gameObject->setIsStatic(configMap->getBool(objectStr+".isStatic"));

    addGameObject(gameObject);
	}

	Memory::free<ConfigMap>(configMap);
}

// ---------------------------------------------------------------------------

void Scene::unloadScene(){
	destroyGameObjects();
}

// ---------------------------------------------------------------------------

void Scene::saveScene(const std::string& path){

	ConfigMap* configMap = Memory::allocate<ConfigMap>();
	configMap->init();

	f32 maxSize = 0;

	u32 counter = 0;
	FOR_LIST(it, mGameObjects){
		if(it.get()->isStatic()){
			// ECHO("SAVE")
			std::string indexStr = std::to_string(counter);
			std::string objectStr = "objects["+indexStr+"]";

			Transform* t = it.get()->getTransform();
			Vector3 worldPosition = t->getWorldPosition();
			Vector3 scale = t->getScale();

			f32 maxObjectScale = std::max(std::abs(scale.x), std::abs(scale.y));
			maxSize = std::max(std::max(maxSize, std::abs(worldPosition.x) + maxObjectScale), std::abs(worldPosition.y) + maxObjectScale);

			Vector3 size = t->getScale();

			Renderer* renderer = it.get()->getComponents<Renderer>()->get(0);
			Texture* texture = renderer->getMaterial()->getTexture();

			configMap->setF32(objectStr+".worldPosition.x", worldPosition.x);
			configMap->setF32(objectStr+".worldPosition.y", worldPosition.y);
			configMap->setF32(objectStr+".size.width", size.x);
			configMap->setF32(objectStr+".size.height", size.y);
			configMap->setString(objectStr+".texture.path", texture->getPath());
			configMap->setF32(objectStr+".texture.region.u", renderer->getRegionPosition().x);
			configMap->setF32(objectStr+".texture.region.v", renderer->getRegionPosition().y);
			configMap->setF32(objectStr+".texture.region.width", renderer->getRegionSize().x);
			configMap->setF32(objectStr+".texture.region.height", renderer->getRegionSize().y);
			configMap->setBool(objectStr+".isStatic", it.get()->isStatic());
			configMap->setU32(objectStr+".layer", renderer->getLayer());

			counter++;
		}
	}

	configMap->setU32("objects.length", counter);
	configMap->setF32("scene.size", maxSize*2.0f);

	configMap->writeConfigFile(path);

	Memory::free<ConfigMap>(configMap);
}

// ---------------------------------------------------------------------------

void Scene::addGameObject(GameObject* gameObject){
	gameObject->setScene(this);
	mNewGameObjects->pushBack(gameObject);
}

// ---------------------------------------------------------------------------

void Scene::updateComponents(GameObject* gameObject){
	List<Script*>* scriptList = gameObject->getComponents<Script>();
	List<Renderer*>* rendererList = gameObject->getComponents<Renderer>();
	List<RigidBody*>* rigidBodyList = gameObject->getComponents<RigidBody>();

	Script* script = scriptList ? scriptList->get(0) : nullptr;
	Renderer* renderer = rendererList ? rendererList->get(0) : nullptr;
	RigidBody* rigidBbody = rigidBodyList ? rigidBodyList->get(0) : nullptr;

	if(script && !script->isAlreadyAddedToEngine()){
		ScriptEngine::getInstance()->addScript(script);
		script->setAlreadyAddedToEngine(true);
	}

	if(rendererList){
		FOR_LIST (it, rendererList){
			if(!it.get()->isAlreadyAddedToEngine()){
				RenderEngine::getInstance()->addRenderer(it.get());
				it.get()->setAlreadyAddedToEngine(true);
			}
		}
	}

	if(rigidBbody && !rigidBbody->isAlreadyAddedToEngine()){
		PhysicsEngine::getInstance()->addRigidBody(rigidBbody);
		rigidBbody->setAlreadyAddedToEngine(true);
	}
}

// ---------------------------------------------------------------------------

void Scene::removeGameObject(GameObject* gameObject){
	auto it = mGameObjects->find(gameObject);
	mGameObjects->remove(it);

	if(!gameObject->isDestroyed()){
		gameObject->destroy();
		gameObject->setDestroyed();
		Memory::free<GameObject>(gameObject);
	}
}

// ---------------------------------------------------------------------------

void Scene::step(){

	if(thereAreNewGameObjects()){

		const List<GameObject*>* newGameObjects = getNewGameObjects();
		u32 maxToSpawn = Settings::getInstance()->getF32("scene.maxNewObjectsToSpawn");

		// VAR(f32, newGameObjects->getLength());

		FOR_LIST (itGameObjects, newGameObjects){
			GameObject* gameObject = itGameObjects.get();

			updateComponents(gameObject);
		}

		flushNewGameObjects();
	}
}
// ---------------------------------------------------------------------------

void Scene::flushNewGameObjects(){
	FOR_LIST (itGameObjects, mNewGameObjects){
		mGameObjects->pushBack(itGameObjects.get());
	}

	mNewGameObjects->clear();
}

// ---------------------------------------------------------------------------

bool Scene::thereAreNewGameObjects() const { return mNewGameObjects->getLength() > 0; }

// ---------------------------------------------------------------------------

} /* namespace DE */