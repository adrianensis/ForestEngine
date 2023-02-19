#include "Editor.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"

void Editor::init()
{
	octree.init(1000);

	SUBSCRIBE_TO_EVENT(InputEventKeyPressed, nullptr, this, [&](const Event *event)
	{
		handlePressedKeys();
	});
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
	// i32 size = 12;
	// for(i32 x = 0; x < size; ++x)
	// {
	// 	for(i32 y = 0; y < size; ++y)
	// 	{
	// 		importModel("resources/bob_lamp/bob_lamp.md5mesh", Vector3(x*100,y*40,0), 1);
	// 		//importModel("resources/char.fbx", Vector3(x*100,y*40,0), 1);
	// 		//createSprite(Vector3(x*40,y*40,0), 30);
	// 	}
	// }

	// importModel("resources/cat_rigged_animation_run.fbx", Vector3(0,0,0), 1);
	// importModel("resources/dragon/untitled.fbx", Vector3(0,0,0), 1);

	//importModel("resources/cat_rigged_animation_run.fbx", Vector3(0,0,0), 1.0f);

	i32 size = 40;
	for(i32 x = -size; x < size; ++x)
	{
		for(i32 y = -size; y < size; ++y)
		{
			//importModel("resources/bob_lamp/bob_lamp_update.fbx", Vector3(x*2,y*2,0), 1);
			//importModel("resources/cat_rigged_animation_run.fbx", Vector3(x,y,0), 1.0f);
			//importModel("resources/Spider.fbx", Vector3(x,y,0),  1);

			
			//importModel("resources/char.fbx", Vector3(x*100,y*40,0), 1);
			//createSprite(Vector3(x,y,0), 1);
		}
	}

    // using namespace ShaderBuilderNodes;
    // using namespace ShaderBuilderNodes::Expressions;

    // ShaderBuilder sb;
    // sb.get().attribute(GPUStorage::UNIFORM, {"float", "time"});
    // sb.get().attribute(GPUStorage::UNIFORM, {"mat4", "viewMatrix"});
    // sb.get().attribute(GPUStorage::UNIFORM, {"mat4", "projectionMatrix"});
    // sb.get().attribute(GPUStorage::UNIFORM, {"vec4", "positionOffset"});
    // auto& isInstanced = sb.get().attribute(GPUStorage::UNIFORM, {"bool", "isInstanced"});
    // auto& hasAnimations = sb.get().attribute(GPUStorage::UNIFORM, {"bool", "hasAnimations"});

    // auto& MAX_BONES = sb.get().attribute(GPUStorage::CONST, {"int", "MAX_BONES", "100"});
    // auto& MAX_BONE_INFLUENCE = sb.get().attribute(GPUStorage::CONST, {"int", "MAX_BONE_INFLUENCE", "4"});

    // auto& gBones = sb.get().attribute(GPUStorage::UNIFORM, {"bool", "gBones", "", MAX_BONES});

    // auto& pos = sb.get().attribute(GPUStorage::IN, 0, {"vec3", "position"});
    // sb.get().attribute(GPUStorage::IN, 1, {"vec2", "texcoord"});
    // sb.get().attribute(GPUStorage::IN, 2, {"vec4", "color"});
    // sb.get().attribute(GPUStorage::IN, 3, {"mat4", "modelMatrix"});
    // auto& bonesIDs = sb.get().attribute(GPUStorage::IN, 7, {"ivec4", "BoneIDs"});
    // auto& Weights = sb.get().attribute(GPUStorage::IN, 8, {"vec4", "Weights"});

    // sb.get().attribute(GPUStorage::OUT, {"vec2", "vTexcoord"});
    // sb.get().attribute(GPUStorage::OUT, {"vec4", "vColor"});

    // auto& mainFunc = sb.get().function("void", "main");

    // Variable totalPosition;
    // Variable localPosition;
    
    // mainFunc.body().
    // variable(totalPosition, "vec4", "totalPosition", call("vec4", {pos, {"1.0f"}}).getNameOrValue()).
    // ifBlock(isInstanced, "&&", hasAnimations).
    //     set(totalPosition, call("vec4", {{"0.0f"}})).
    //     forBlock("i", "<", MAX_BONE_INFLUENCE, "++").
    //         ifBlock(bonesIDs.at("i"), "==", {"-1"}).
    //             line("continue").
    //         end().
    //         ifBlock(bonesIDs.at("i"), ">=", MAX_BONES).
    //             set(totalPosition, call("vec4", {pos, {"1.0f"}})).
    //             line("break").
    //         end().
    //         variable(localPosition, "vec4", "localPosition", gBones.at(bonesIDs.at("i")).mul(call("vec4", {pos, {"1.0f"}})).getNameOrValue()).
    //         set(totalPosition, totalPosition.add(localPosition.mul(Weights.at("i")))).
    //     end().
    // end();

    // std::string code = sb.getCode();
    // ECHO(code);

	importModel("bob_lamp/bob_lamp_update.fbx", Vector3(0,0,-5), 1.0f);
	importModel2("bob_lamp/bob_lamp_update.fbx", Vector3(10,0,0), 1.0f);
	importModel2("bob_lamp/bob_lamp_update.fbx", Vector3(15,0,0), 1.0f);
	//importModel2("dragon/Dragon_2.5_For_Animations.fbx", Vector3(15,0,0), 1.0f);
	// importModel("resources/bob_lamp/bob_lamp_update.fbx", Vector3(50,0,50), 5);
	// importModel("resources/bob_lamp/bob_lamp_update.fbx", Vector3(0,0,0), 5);

	//importModel("resources/Player.fbx", Vector3(0,0,0),  1);
	// importModel("resources/dancing_vampire.dae", Vector3(0,0,0),  1);
	//importModel("resources/bob_lamp/bob_lamp_update_export.md5mesh", Vector3(0,0,0), 1);

	//importModel("resources/rp_nathan_animated_003_walking.fbx", Vector3(0,0,0),  1);
	//importModel("resources/cube/cube.fbx", Vector3(0,0,0),  1);
	// importModel("resources/Spider.fbx", Vector3(0,0,0),  1);

	//importModel("resources/wolf/Wolf_One_fbx7.4_binary.fbx");
	//importModel("resources/bob_lamp/bob_lamp.md5mesh", Vector3(0,0,0), 1);
	//importModel("resources/cs_havana.obj", Vector3(0,0,0),  1);


	UIBuilder uiBuilder;

	uiBuilder.
	setPosition(Vector2(-1,1)).
	setAdjustSizeToText(true).
	setSize(Vector2(0.5f, 0.05f));

	uiBuilder.
	setText("File").
	create<UIDropdown>().
	getUIElement<UIDropdown>().get().
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
	getUIElement<UIButton>().get().
	setOnPressedCallback([&, this](UIElement *uiElement){
	});

	uiBuilder.
	setText("View").
	create<UIDropdown>().
	getUIElement<UIDropdown>().get().
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
	setMaterial(GET_SYSTEM(MaterialManager).createMaterialWithTexture("resources/editor-icons/EventPoint.png")).
	create<UIToggleButton>().
	getUIElement<UIToggleButton>().get().
	setOnPressedCallback([&](UIElement* uiElement){
	});

	uiBuilder.
	getUIElement<UIToggleButton>().get().simulateClick();

	uiBuilder.
	setMaterial(GET_SYSTEM(MaterialManager).createMaterialWithTexture("resources/editor-icons/PlayerStart.png")).
	create<UIToggleButton>().
	getUIElement<UIToggleButton>().get().
	setOnPressedCallback([&](UIElement* uiElement){
	});

	uiBuilder.
	nextRow();

	uiBuilder.
	setMaterial(GET_SYSTEM(MaterialManager).createMaterialWithTexture("resources/editor-icons/SpawnPoint.png")).
	create<UIToggleButton>().
	getUIElement<UIToggleButton>().get().
	setOnPressedCallback([&](UIElement* uiElement){
	});

	uiBuilder.
	setMaterial(GET_SYSTEM(MaterialManager).createMaterialWithTexture("resources/editor-icons/WayPoint.png")).
	create<UIToggleButton>().
	getUIElement<UIToggleButton>().get().
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
	getUIElement<UIDropdown>().get().
	addOption("Option1", [&](UIElement *uiElement)
	{

	}).
	addOption("Option2", [&](UIElement *uiElement)
	{
	}).
	addOption("Option3", [&](UIElement *uiElement)
	{
	});

	uiBuilder.
	nextRow();

	uiBuilder.
	setText("Test Button").
	setAdjustSizeToText(false).
	setSize(Vector2(0.1f, 0.3f)).
	create<UIList>().
	getUIElement<UIList>().get().
	addOption("Option 1", [](UIElement *uiElement){ ECHO("OPTION 1") }).
	addOption("Option 2", [](UIElement *uiElement){ ECHO("OPTION 2") }).
	addOption("Option 3", [](UIElement *uiElement){ ECHO("OPTION 3") }).
	toggle();

	cameraGameObject = GET_SYSTEM(ScenesManager).getCurrentScene().get().getCameraGameObject();
}

void Editor::update()
{
	PROFILER_CPU()

	Vector2 currentMousePosition = GET_SYSTEM(Input).getMousePosition();




	rotation += 1.0f;


	


	



	y += 30 * GET_SYSTEM(Time).getDeltaTimeSeconds();


	Transform* cameraTransform = &cameraGameObject.get().mTransform.get();
	Matrix4 cameraRotationMatrix = cameraGameObject.get().mTransform.get().getRotationMatrix();
	cameraRotationMatrix.invert();

	f32 speed = 90 * GET_SYSTEM(Time).getDeltaTimeSeconds();

	if(GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_LEFT))
	{
		cameraTransform->translate(cameraRotationMatrix.mulVector(Vector3(-speed,0,0)));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_RIGHT))
	{
		cameraTransform->translate(cameraRotationMatrix.mulVector(Vector3(speed,0,0)));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_UP))
	{
		cameraTransform->translate(cameraRotationMatrix.mulVector(Vector3(0,0,-speed)));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_DOWN))
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


	GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(-1000,0,0), Vector3(1000,0,0)), 2, true, Vector4(1,0,0,1));

	GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(1000,0,0), Vector3(1000,100,0)), 1, true, Vector4(1,0,0,1));
	GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(-1000,0,0), Vector3(-1000,100,0)), 1, true, Vector4(1,1,0,1));

	GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,-1000,0), Vector3(0,1000,0)), 2, true, Vector4(0,1,0,1));

	GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,0,-1000), Vector3(0,0,1000)), 2, true, Vector4(0,0,1,1));

	GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,0,1000), Vector3(0,100,1000)), 1, true, Vector4(0,0,1,1));
	GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,0,-1000), Vector3(0,100,-1000)), 1, true, Vector4(0,1,1,1));

	for(i32 x = -1000; x < 1000; x+=50)
	{
		GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(x,0,-1000), Vector3(x,0,1000)), 1, true, Vector4(0,0,1,0.3f));
	}


	octree.update();
}

void Editor::terminate()
{
	
}

OwnerPtr<GameObject> Editor::createSprite(const Vector3& v, f32 size)
{
	OwnerPtr<GameObject> gameObject = OwnerPtr<GameObject>::newObject();
	gameObject.get().init();
	gameObject.get().mIsStatic = false;
	gameObject.get().mTransform.get().mLocalPosition = (v);
	gameObject.get().mTransform.get().mScale = (Vector3(size,size,size));

	OwnerPtr<Renderer> renderer = OwnerPtr<Renderer>::newObject();
	renderer.get().init();

	renderer.get().mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Cube>();
	renderer.get().mMaterial = (GET_SYSTEM(MaterialManager).createMaterialWithTexture("resources/snorlax-fill.png"));

	gameObject.get().addComponent<Renderer>(renderer);

	GET_SYSTEM(ScenesManager).getCurrentScene().get().addGameObject(gameObject);

	return gameObject;
}

void Editor::importModel( const std::string& pFile, const Vector3& v, f32 size)
{
	Ptr<const Model> model = GET_SYSTEM(ModelManager).loadModel(pFile);

	OwnerPtr<GameObject> gameObject = OwnerPtr<GameObject>::newObject();
	gameObject.get().init();
	gameObject.get().mIsStatic = true;
	gameObject.get().mTransform.get().mLocalPosition = (v);
	gameObject.get().mTransform.get().mScale = (Vector3(1,1,1) * size);
	//gameObject->mTransform.get().setRotation(Vector3(90,0,0));

	OwnerPtr<ModelRenderer > modelRenderer = OwnerPtr<ModelRenderer>::newObject();
	modelRenderer.get().mModel = (model);
	modelRenderer.get().mIsInstanced = (true);
	modelRenderer.get().mStencilValue = (0x1);
	modelRenderer.get().mIsStencilMask = (true);
	
	gameObject.get().addComponent<ModelRenderer>(modelRenderer);

	GET_SYSTEM(ScenesManager).getCurrentScene().get().addGameObject(gameObject);
}

void Editor::importModel2( const std::string& pFile, const Vector3& v, f32 size)
{
	Ptr<const Model> model = GET_SYSTEM(ModelManager).loadModel(pFile);

	OwnerPtr<GameObject> gameObject = OwnerPtr<GameObject>::newObject();
	gameObject.get().init();
	gameObject.get().mIsStatic = true;
	gameObject.get().mTransform.get().mLocalPosition = (v);
	gameObject.get().mTransform.get().mScale = (Vector3(1,1,1) * size);
	//gameObject->mTransform.get().setRotation(Vector3(90,0,0));

	OwnerPtr<ModelRenderer > modelRenderer = OwnerPtr<ModelRenderer>::newObject();
	modelRenderer.get().mModel = (model);
	modelRenderer.get().mIsInstanced = (true);
	modelRenderer.get().mStencilValue = (0x1);
	
	gameObject.get().addComponent<ModelRenderer>(modelRenderer);

	GET_SYSTEM(ScenesManager).getCurrentScene().get().addGameObject(gameObject);
}

void Editor::handlePressedKeys()
{
	if(GET_SYSTEM(Input).isKeyPressedOnce(GLFW_KEY_KP_ADD))
	{
	}

	if(GET_SYSTEM(Input).isKeyPressedOnce(GLFW_KEY_KP_SUBTRACT))
	{
	}
}
