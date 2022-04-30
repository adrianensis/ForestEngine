#pragma once

#include "Scripting/Script.hpp"
#include "UI/UIStyle.hpp"

#ifdef CPP_INCLUDE
#include "Editor.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#endif

class UIStyleEditorToolButton: public UIStyleDefault
{
    GENERATE_METADATA(UIStyleEditorToolButton)

public:
	UIStyleEditorToolButton()
	{
		mBackgroundColor = Vector4(0.0f, 0.0f, 0.0f, 1);
		mColorPressed = Vector4(-0.3f, -0.3f, -0.3f, 1);
		mColorHovered = Vector4(0.4f, 0.4f, 0.4f, 1);
	}
};

class Editor: public Script
{
    GENERATE_METADATA(Editor)

public:
	CPP void init()
	{

	}

	CPP void firstUpdate()
	{
		i32 size = 4;
		for(i32 x = 0; x < size; ++x)
		{
			sprite = createSprite(Vector3(x*100,0,x*100), 50);
		}

		//sprite = createSprite(Vector3(0,0,0), 50);

		UIBuilder uiBuilder;

		uiBuilder.
		setPosition(Vector2(-1,1)).
		setAdjustSizeToText(true).
		setSize(Vector2(0.5f, 0.05f));

		uiBuilder.
		setText("File").
		create<UIDropdown>().
		getUIElement<UIDropdown>()->
		addOption("New", [&](UIElement *uiElement)
		{

		}).
		addOption("Open", [&](UIElement *uiElement)
		{
		}).
		addOption("Save", [&](UIElement *uiElement)
		{
		});

		uiBuilder.
		setText("Sprites").
		create<UIButton>().
		getUIElement<UIButton>()->
		setOnPressedCallback([&, this](UIElement *uiElement){
		});

		uiBuilder.
		setText("View").
		create<UIDropdown>().
		getUIElement<UIDropdown>()->
		addOption("Grid", [&](UIElement *uiElement)
		{

		}).
		addOption("Colliders", [&](UIElement *uiElement)
		{
		});

		uiBuilder.restoreAll();

		uiBuilder.
		setLayout(UILayout::HORIZONTAL).
		setPosition(Vector2(-1,0.5f)).
		setAdjustSizeToText(false).
		setStyle(&UIStyleManager::getInstance().getOrAddStyle<UIStyleEditorToolButton>()).
		setGroup("toggleButtons").
		setSize(Vector2(0.1f, 0.1f));

		uiBuilder.
		setMaterial(MaterialManager::getInstance().loadMaterial("resources/editor-icons/EventPoint.png")).
		create<UIToggleButton>().
		getUIElement<UIToggleButton>()->
		setOnPressedCallback([&](UIElement* uiElement){
		});

		uiBuilder.
		getUIElement<UIToggleButton>()->simulateClick();

		uiBuilder.
		setMaterial(MaterialManager::getInstance().loadMaterial("resources/editor-icons/PlayerStart.png")).
		create<UIToggleButton>().
		getUIElement<UIToggleButton>()->
		setOnPressedCallback([&](UIElement* uiElement){
		});

		uiBuilder.
		nextRow();

		uiBuilder.
		setMaterial(MaterialManager::getInstance().loadMaterial("resources/editor-icons/SpawnPoint.png")).
		create<UIToggleButton>().
		getUIElement<UIToggleButton>()->
		setOnPressedCallback([&](UIElement* uiElement){
		});

		uiBuilder.
		setMaterial(MaterialManager::getInstance().loadMaterial("resources/editor-icons/WayPoint.png")).
		create<UIToggleButton>().
		getUIElement<UIToggleButton>()->
		setOnPressedCallback([&](UIElement* uiElement){
		});

		uiBuilder.
		nextRow();

		uiBuilder.restoreMaterial();
		uiBuilder.restoreStyle();
		uiBuilder.restoreGroup();

		uiBuilder.
		setText("Test Button").
		setAdjustSizeToText(true).
		create<UIDropdown>().
		getUIElement<UIDropdown>()->
		addOption("Option1", [&](UIElement *uiElement)
		{

		}).
		addOption("Option2", [&](UIElement *uiElement)
		{
		}).
		addOption("Option3", [&](UIElement *uiElement)
		{
		});

		cameraGameObject = ScenesManager::getInstance().getCurrentScene()->getCameraGameObject();
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
		//sprite->getTransform().get().translate(Vector3(-y, 0, 0));

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
		y += 30 * Time::getInstance().getDeltaTimeSeconds();

		//sprite->getTransform().get().setLocalPosition(position);

		Transform* cameraTransform = &cameraGameObject->getTransform().get();

		f32 speed = 200 * Time::getInstance().getDeltaTimeSeconds();

		if(Input::getInstance().isKeyPressed(GLFW_KEY_LEFT))
		{
			cameraTransform->translate(Vector3(-speed,0,0));
		}
		else if (Input::getInstance().isKeyPressed(GLFW_KEY_RIGHT))
		{
			cameraTransform->translate(Vector3(speed,0,0));
		}
		else if (Input::getInstance().isKeyPressed(GLFW_KEY_UP))
		{
			cameraTransform->translate(Vector3(0,0,speed));
		}
		else if (Input::getInstance().isKeyPressed(GLFW_KEY_DOWN))
		{
			cameraTransform->translate(Vector3(0,0,-speed));
		}

		if(Input::getInstance().isKeyPressed(GLFW_KEY_W))
		{
			cameraTransform->rotate(Vector3(-speed,0,0));
		}
		else if (Input::getInstance().isKeyPressed(GLFW_KEY_S))
		{
			cameraTransform->rotate(Vector3(speed,0,0));
		}
		else if (Input::getInstance().isKeyPressed(GLFW_KEY_A))
		{
			cameraTransform->rotate(Vector3(0,speed,0));
		}
		else if (Input::getInstance().isKeyPressed(GLFW_KEY_D))
		{
			cameraTransform->rotate(Vector3(0,-speed,0));
		}


		// x
		RenderEngine::getInstance().drawLine(Line(Vector3(-1000,-10,0), Vector3(1000,-10,0)), 2, true, Vector4(1,0,0,1));

		// +x
		RenderEngine::getInstance().drawLine(Line(Vector3(1000,0,0), Vector3(1000,100,0)), 1, true, Vector4(1,0,0,1));
		// -x
		RenderEngine::getInstance().drawLine(Line(Vector3(-1000,0,0), Vector3(-1000,100,0)), 1, true, Vector4(1,1,0,1));

		// y
		RenderEngine::getInstance().drawLine(Line(Vector3(0,-1000,0), Vector3(0,1000,0)), 2, true, Vector4(0,1,0,1));

		// z
		RenderEngine::getInstance().drawLine(Line(Vector3(0,-10,-1000), Vector3(0,-10,1000)), 2, true, Vector4(0,0,1,1));

		// +z
		RenderEngine::getInstance().drawLine(Line(Vector3(0,0,1000), Vector3(0,100,1000)), 1, true, Vector4(0,0,1,1));
		// -z
		RenderEngine::getInstance().drawLine(Line(Vector3(0,0,-1000), Vector3(0,100,-1000)), 1, true, Vector4(0,1,1,1));

		// floor
		for(i32 x = -1000; x < 1000; x+=50)
		{
			RenderEngine::getInstance().drawLine(Line(Vector3(x,-10,-1000), Vector3(x,-10,1000)), 1, true, Vector4(0,0,1,1));
		}

		// for(i32 z = -1000; z < 1000; z+=50)
		// {
		// 	RenderEngine::getInstance().drawLine(Line(Vector3(-1000,-10,z), Vector3(1000,-10,z)), 1, true, Vector4(1,0,0,1));
		// }
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
	GameObject* cameraGameObject = nullptr;
};