#pragma once

#include "Scripting/Script.hpp"

#ifdef CPP_INCLUDE
#include "Editor.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#endif

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
			//createSprite(Vector3(x,0,x));
		}

		sprite = createSprite(Vector3(0,0,0), 50);

		// UIBuilder uiBuilder;

		// uiBuilder.
		// setLayout(UILayout::HORIZONTAL).
		// setPosition(Vector2(-1,1)).
		// setLayer(0).
		// setAdjustSizeToText(true).
		// setSize(Vector2(0.5f, 0.05f));

		// uiBuilder.
		// setText("File").
		// create<UIDropdown>().
		// getUIElement<UIDropdown>()->
		// addOption("New", [&](UIElement *uiElement)
		// {

		// }).
		// addOption("Open", [&](UIElement *uiElement)
		// {
		// }).
		// addOption("Save", [&](UIElement *uiElement)
		// {
		// });

		// uiBuilder.
		// setText("Grid").
		// create<UIButton>().
		// getUIElement<UIButton>()->
		// setOnPressedCallback([&, this](UIElement *uiElement){
		// });

		// uiBuilder.
		// setText("Sprites").
		// create<UIButton>().
		// getUIElement<UIButton>()->
		// setOnPressedCallback([&, this](UIElement *uiElement){
		// });

		
	}

	CPP void update()
	{
		// Vector3 localPosition = position;

		// Matrix4 rotationMatrix;
		// rotationMatrix.rotation(Vector3(rotation,rotation,rotation));

		// RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x + 0.1f ,localPosition.y,localPosition.z, 1))), 1, false, Vector4(1,0,0,1));
		// RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x,localPosition.y + 0.1f ,localPosition.z, 1))), 1, false, Vector4(0,1,0,1));
		// RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x,localPosition.y,localPosition.z + 0.1f , 1))), 1, false, Vector4(0,0,1,1));

		sprite->getTransform().get().setRotation(Vector3(rotation, rotation, 0));
		//sprite->getTransform().get().translate(Vector3(0, 0, y));

		rotation += 1.0f;

		// RenderEngine::getInstance().drawLine(Line(Vector3(0,0,0), position), 1, true, Vector4(1,0,0,1));

		

		// Line ray(Vector3(-500, -500,0), Vector3(500,y,0));
		// RenderEngine::getInstance().drawLine(ray, 1, true, Vector4(1,0,0,1));

		// Vector3 intersectionResult1;
		// Vector3 intersectionResult2;
		// u8 intersectionsCount = Geometry::testLineSphere(ray, Sphere(Vector3(0,0,0), 300), 0, intersectionResult1, intersectionResult2);
		
		// if(intersectionsCount > 0)
		// {
		// 	createSprite(intersectionResult1, 5);

		// 	if(intersectionsCount > 1)
		// 	{
		// 		createSprite(intersectionResult2, 5);
		// 	}
		// }

		// Vector3 intersectionResultLines;
		// Line ray2(Vector3(0, -100,0), Vector3(0,1000,0));
		// RenderEngine::getInstance().drawLine(ray2, 1, true, Vector4(1,0,0,1));
		// if(Geometry::testLineLine(ray2, ray, intersectionResultLines, 0))
		// {
		// 	createSprite(intersectionResultLines, 5);
		// }

		// position.x += 1;
		y += 5;

		//sprite->getTransform().get().setLocalPosition(position);
	}

	CPP void terminate()
	{
		
	}

private:
	CPP GameObject* createSprite(const Vector3& v, f32 size)
	{
		GameObject* gameObject = NEW(GameObject);
		gameObject->init();
		gameObject->setIsStatic(false);
		gameObject->getTransform().get().setLocalPosition(v);
		gameObject->getTransform().get().setScale(Vector3(size,size,size));

		Renderer *renderer = NEW(Renderer);
		renderer->init();

		renderer->setMesh(MeshPrimitives::getInstance().getPrimitive<Cube>());
		renderer->setMaterial(MaterialManager::getInstance().loadMaterial("resources/snorlax-fill.png"));
		//renderer->setColor(Vector4(1,1,0,1));

		gameObject->addComponent<Renderer>(renderer);

		ScenesManager::getInstance().getCurrentScene()->addGameObject(gameObject);

		return gameObject;
	}

private:
	f32 rotation = 0;
	f32 y = 0;
	Vector3 position;
	GameObject* sprite = nullptr;
};