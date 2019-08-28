#include "CustomScript.h"
#include "Debug.h"

#include "GameObject.h"
#include "Transform.h"
#include "Renderer.h"
#include "Vector3.h"
#include "Time.h"
#include "Input.h"
#include "List.h"

#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Animation.h"

#include "Scene.h"

namespace DE {

// ---------------------------------------------------------------------------

CustomScript::CustomScript() : Script(){

}

// ---------------------------------------------------------------------------

CustomScript::~CustomScript() = default;

// ---------------------------------------------------------------------------

void CustomScript::init(){
  mRenderer = getGameObject()->getComponents<Renderer>()->get(0);
  mTransform = getGameObject()->getTransform();
  mTestObjCreated = false;

    Texture* texture = Memory::allocate<Texture>();
  	texture->init("resources/mage.bmp");

  	Material* material = Memory::allocate<Material>();
  	material->init();
  	material->setTexture(texture);

    obj = Memory::allocate<GameObject>();
    obj->init();

    obj->getTransform()->setLocalPosition(Vector3(0,0,0));
    obj->getTransform()->setScale(Vector3(300,300,1));

    Renderer* renderer = Memory::allocate<Renderer>();
    obj->addComponent<Renderer>(renderer);

    renderer->setMesh(Mesh::getRectangle());
    renderer->setMaterial(material);

    renderer->addAnimation("idle", Animation::create(6, true, false, Vector2(0,0), 1.0f/6.0f, 1.0f/2.0f, 10));
  	renderer->addAnimation("run", Animation::create(6, true, false, Vector2(0,0.5), 1.0f/6.0f, 1.0f/2.0f, 10));
  	renderer->setAnimation("idle");
}

// ---------------------------------------------------------------------------

void CustomScript::step(){

  f32 movement = 1.0f*Time::getDeltaTimeSeconds();

  bool running = false;

  if(Input::isKeyPressed(GLFW_KEY_UP))
  {
    mTransform->translate(Vector3(0,movement,0));
    running = true;
    // mRenderer->setInvertXAxis(false);
  }
  else if(Input::isKeyPressed(GLFW_KEY_DOWN))
  {
    mTransform->translate(Vector3(0,-movement,0));
    running = true;
    // mRenderer->setInvertXAxis(false);
  }
  else if(Input::isKeyPressed(GLFW_KEY_LEFT))
  {
    mTransform->translate(Vector3(-movement,0,0));
    running = true;
    mRenderer->setInvertXAxis(true);
  }
  else if(Input::isKeyPressed(GLFW_KEY_RIGHT))
  {
    mTransform->translate(Vector3(movement,0,0));
    running = true;
    mRenderer->setInvertXAxis(false);
  }
  else if(Input::isKeyPressed(GLFW_KEY_ENTER))
  {
    if(!mTestObjCreated) {
      getGameObject()->getScene()->addGameObject(obj);
      mTestObjCreated = true;
    }
  }


  if(running){
    mRenderer->setAnimation("run");
  }else{
    mRenderer->setAnimation("idle");
  }
}

// ---------------------------------------------------------------------------

void CustomScript::terminate() {

}

// ---------------------------------------------------------------------------

} /* namespace DE */
