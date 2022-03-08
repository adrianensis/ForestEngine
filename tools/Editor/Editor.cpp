#include "Editor.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"

void Editor::init()
{

}

void Editor::firstUpdate()
{
   createSprite(Vector3(0,0,0));
   createSprite(Vector3(0,300,0));
   createSprite(Vector3(300,0,0));
}

void Editor::update()
{
	Vector3 localPosition(0,0,0);

    Matrix4 rotationMatrix;
    rotationMatrix.rotation(Vector3(rotation,rotation,rotation));

    RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x + 0.1f ,localPosition.y,localPosition.z, 1))), 1, false, Vector4(1,0,0,1));
	RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x,localPosition.y + 0.1f ,localPosition.z, 1))), 1, false, Vector4(0,1,0,1));
	RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x,localPosition.y,localPosition.z + 0.1f , 1))), 1, false, Vector4(0,0,1,1));

    rotation += 1.0f;
}

void Editor::terminate()
{
	
}

void Editor::createSprite(const Vector3& v)
{
    GameObject* gameObject = NEW(GameObject);
    gameObject->init();
	gameObject->getTransform()->setLocalPosition(v);
	gameObject->getTransform()->setScale(Vector3(100.0f,100.0f,1.0f));

	Renderer *renderer = NEW(Renderer);
	renderer->init();

	renderer->setMesh(MeshPrimitives::getInstance().getPrimitive<Rectangle>());
	renderer->setMaterial(MaterialManager::getInstance().loadNoTextureMaterial());
	renderer->setColor(Vector4(1,1,0,1));

	gameObject->addComponent<Renderer>(renderer);

    ScenesManager::getInstance().getCurrentScene()->addGameObject(gameObject);
}