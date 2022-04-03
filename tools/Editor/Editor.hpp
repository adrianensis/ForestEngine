#pragma once

#ifdef CPP_INCLUDE
#include "Editor.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#endif

#include "Scripting/Script.hpp"

class Editor: public Script
{
    GENERATE_METADATA(Editor)

public:
	CPP void init()
	{

	}

	CPP void firstUpdate()
	{
		i32 size = 101;
		for(i32 x = 0; x < size; ++x)
		{
			createSprite(Vector3(x,0,0));
		}
		
	}

	CPP void update()
	{
		Vector3 localPosition(0,0,0);

		Matrix4 rotationMatrix;
		rotationMatrix.rotation(Vector3(rotation,rotation,rotation));

		RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x + 0.1f ,localPosition.y,localPosition.z, 1))), 1, false, Vector4(1,0,0,1));
		RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x,localPosition.y + 0.1f ,localPosition.z, 1))), 1, false, Vector4(0,1,0,1));
		RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x,localPosition.y,localPosition.z + 0.1f , 1))), 1, false, Vector4(0,0,1,1));

		rotation += 1.0f;
	}

	CPP void terminate()
	{
		
	}

private:
	CPP void createSprite(const Vector3& v)
	{
		GameObject* gameObject = NEW(GameObject);
		gameObject->init();
		gameObject->setIsStatic(true);
		gameObject->getTransform().get().setLocalPosition(v);
		gameObject->getTransform().get().setScale(Vector3(100.0f,100.0f,1.0f));

		Renderer *renderer = NEW(Renderer);
		renderer->init();

		renderer->setMesh(MeshPrimitives::getInstance().getPrimitive<Rectangle>());
		renderer->setMaterial(MaterialManager::getInstance().loadMaterial("resources/mountain.png"));
		renderer->setColor(Vector4(1,1,0,1));

		gameObject->addComponent<Renderer>(renderer);

		ScenesManager::getInstance().getCurrentScene()->addGameObject(gameObject);
	}

private:
	f32 rotation = 0;
	f32 y = 0;

};