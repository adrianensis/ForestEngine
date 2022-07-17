#include "Editor.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"

void Editor::init()
{
	octree.init(1000);
}

void Editor::firstUpdate()
{

	// i32 size = 7;
	// for(i32 x = 0; x < size; ++x)
	// {
	// 	for(i32 y = 0; y < size; ++y)
	// 	{
	// 		for(i32 z = 0; z < size; ++z)
	// 		{
	// 			importModel("resources/bob_lamp/bob_lamp.md5mesh", Vector3(x*20,y*20,z*20), 1);
	// 		}
	// 	}
	// }
	i32 size = 12;
	for(i32 x = 0; x < size; ++x)
	{
		for(i32 y = 0; y < size; ++y)
		{
			importModel("resources/bob_lamp/bob_lamp.md5mesh", Vector3(x*100,y*40,0), 1);
			//importModel("resources/char.fbx", Vector3(x*100,y*40,0), 1);
			//createSprite(Vector3(x*40,y*40,0), 30);
		}
	}

	//importModel("resources/bob_lamp/bob_lamp.md5mesh", Vector3(-100,0,0), 1);

	//importModel("resources/wolf/Wolf_One_fbx7.4_binary.fbx");
	//importModel("resources/bob_lamp/bob_lamp.md5mesh", Vector3(0,0,0), 1);
	//importModel("resources/cs_havana.obj");


	// UIBuilder uiBuilder;

	// uiBuilder.
	// setPosition(Vector2(-1,1)).
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
	// setText("Sprites").
	// create<UIButton>().
	// getUIElement<UIButton>()->
	// setOnPressedCallback([&, this](UIElement *uiElement){
	// });

	// uiBuilder.
	// setText("View").
	// create<UIDropdown>().
	// getUIElement<UIDropdown>()->
	// addOption("Grid", [&](UIElement *uiElement)
	// {

	// }).
	// addOption("Colliders", [&](UIElement *uiElement)
	// {
	// });

	// uiBuilder.restoreAll();

	// uiBuilder.
	// setLayout(UILayout::HORIZONTAL).
	// setPosition(Vector2(-1,0.5f)).
	// setAdjustSizeToText(false).
	// setStyle(&UIStyleManager::getInstance().getOrAddStyle<UIStyleEditorToolButton>()).
	// setGroup("toggleButtons").
	// setSize(Vector2(0.1f, 0.1f));

	// uiBuilder.
	// setMaterial(MaterialManager::getInstance().loadMaterial("resources/editor-icons/EventPoint.png")).
	// create<UIToggleButton>().
	// getUIElement<UIToggleButton>()->
	// setOnPressedCallback([&](UIElement* uiElement){
	// });

	// uiBuilder.
	// getUIElement<UIToggleButton>()->simulateClick();

	// uiBuilder.
	// setMaterial(MaterialManager::getInstance().loadMaterial("resources/editor-icons/PlayerStart.png")).
	// create<UIToggleButton>().
	// getUIElement<UIToggleButton>()->
	// setOnPressedCallback([&](UIElement* uiElement){
	// });

	// uiBuilder.
	// nextRow();

	// uiBuilder.
	// setMaterial(MaterialManager::getInstance().loadMaterial("resources/editor-icons/SpawnPoint.png")).
	// create<UIToggleButton>().
	// getUIElement<UIToggleButton>()->
	// setOnPressedCallback([&](UIElement* uiElement){
	// });

	// uiBuilder.
	// setMaterial(MaterialManager::getInstance().loadMaterial("resources/editor-icons/WayPoint.png")).
	// create<UIToggleButton>().
	// getUIElement<UIToggleButton>()->
	// setOnPressedCallback([&](UIElement* uiElement){
	// });

	// uiBuilder.
	// nextRow();

	// uiBuilder.restoreMaterial();
	// uiBuilder.restoreStyle();
	// uiBuilder.restoreGroup();

	// uiBuilder.
	// setText("Test Button").
	// setAdjustSizeToText(true).
	// create<UIDropdown>().
	// getUIElement<UIDropdown>()->
	// addOption("Option1", [&](UIElement *uiElement)
	// {

	// }).
	// addOption("Option2", [&](UIElement *uiElement)
	// {
	// }).
	// addOption("Option3", [&](UIElement *uiElement)
	// {
	// });

	cameraGameObject = ScenesManager::getInstance().getCurrentScene()->getCameraGameObject();
}

void Editor::update()
{
	PROFILER_CPU()

	Vector2 currentMousePosition = Input::getInstance().getMousePosition();




	rotation += 1.0f;


	


	



	y += 30 * Time::getInstance().getDeltaTimeSeconds();


	Transform* cameraTransform = &cameraGameObject->getTransform().get();
	Matrix4 cameraRotationMatrix = cameraGameObject->getTransform().get().getRotationMatrix();
	cameraRotationMatrix.invert();

	f32 speed = 200 * Time::getInstance().getDeltaTimeSeconds();

	if(Input::getInstance().isKeyPressed(GLFW_KEY_LEFT))
	{
		cameraTransform->translate(cameraRotationMatrix.mulVector(Vector3(-speed,0,0)));
	}
	else if (Input::getInstance().isKeyPressed(GLFW_KEY_RIGHT))
	{
		cameraTransform->translate(cameraRotationMatrix.mulVector(Vector3(speed,0,0)));
	}
	else if (Input::getInstance().isKeyPressed(GLFW_KEY_UP))
	{
		cameraTransform->translate(cameraRotationMatrix.mulVector(Vector3(0,0,-speed)));
	}
	else if (Input::getInstance().isKeyPressed(GLFW_KEY_DOWN))
	{
		cameraTransform->translate(cameraRotationMatrix.mulVector(Vector3(0,0,speed)));
	}

	if(!mousePosition.eq(currentMousePosition))
	{
		Vector2 mouseVector = (mousePosition-currentMousePosition).nor() * speed;
		Vector3 direction;

		f32 yaw = -mouseVector.x;
		f32 pitch = mouseVector.y;

		cameraTransform->rotate(-Vector3(pitch, yaw, 0));
	}

	mousePosition = currentMousePosition;


	RenderEngine::getInstance().drawLine(Line(Vector3(-1000,0,0), Vector3(1000,0,0)), 2, true, Vector4(1,0,0,1));

	RenderEngine::getInstance().drawLine(Line(Vector3(1000,0,0), Vector3(1000,100,0)), 1, true, Vector4(1,0,0,1));
	RenderEngine::getInstance().drawLine(Line(Vector3(-1000,0,0), Vector3(-1000,100,0)), 1, true, Vector4(1,1,0,1));

	RenderEngine::getInstance().drawLine(Line(Vector3(0,-1000,0), Vector3(0,1000,0)), 2, true, Vector4(0,1,0,1));

	RenderEngine::getInstance().drawLine(Line(Vector3(0,0,-1000), Vector3(0,0,1000)), 2, true, Vector4(0,0,1,1));

	RenderEngine::getInstance().drawLine(Line(Vector3(0,0,1000), Vector3(0,100,1000)), 1, true, Vector4(0,0,1,1));
	RenderEngine::getInstance().drawLine(Line(Vector3(0,0,-1000), Vector3(0,100,-1000)), 1, true, Vector4(0,1,1,1));

	for(i32 x = -1000; x < 1000; x+=50)
	{
		RenderEngine::getInstance().drawLine(Line(Vector3(x,0,-1000), Vector3(x,0,1000)), 1, true, Vector4(0,0,1,0.3f));
	}


	octree.update();

	f32 sizeVector = sizeof(Vector3);
	f32 sizeFloat = sizeof(f32);

	VAR(sizeVector/sizeFloat)
}

void Editor::terminate()
{
	
}

GameObject* Editor::createSprite(CR(Vector3) v, f32 size)
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

	gameObject->addComponent<Renderer>(renderer);

	ScenesManager::getInstance().getCurrentScene()->addGameObject(gameObject);

	return gameObject;
}

void Editor::importModel( CR(std::string) pFile, CR(Vector3) v, f32 size)
{
	Ptr<const Model> model = ModelManager::getInstance().loadModel(pFile);

	GameObject* gameObject = NEW(GameObject);
	gameObject->init();
	gameObject->setIsStatic(true);
	gameObject->getTransform().get().setLocalPosition(v);
	gameObject->getTransform().get().setScale(Vector3(1,1,1) * size);

	CR(std::vector<OwnerPtr<Mesh>>) meshes = model.get().getMeshes();

	FOR_LIST(it, meshes)
	{
		Renderer *renderer = NEW(Renderer);
		renderer->init();

		renderer->setMesh(*it);
		renderer->setMaterial(MaterialManager::getInstance().loadMaterial("resources/bob_lamp/" + (*it).get().getMaterialPath()));
		//renderer->setMaterial(MaterialManager::getInstance().loadMaterial("resources/wolf/textures/Wolf_Body.png"));
		//renderer->setMaterial(MaterialManager::getInstance().loadMaterial("resources/bob_lamp/guard1_body.png"));
		//renderer->setMaterial(MaterialManager::getInstance().loadNoTextureMaterial());
		// renderer->setColor(Vector4(0,std::sin(Time::getInstance().getElapsedTimeMillis())+0.2f,0,1));
		//renderer->setColor(Vector4(0,1,0,1));

		gameObject->addComponent<Renderer>(renderer);
	}

	ScenesManager::getInstance().getCurrentScene()->addGameObject(gameObject);
}
