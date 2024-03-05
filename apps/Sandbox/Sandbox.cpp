#include "Sandbox.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"

void Sandbox::init()
{
    PROFILER_CPU();
	SUBSCRIBE_TO_EVENT(InputEventKeyPressed, nullptr, this, [&](const Event *event)
	{
		handlePressedKeys();
	});
}

void Sandbox::firstUpdate()
{
    PROFILER_CPU();
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

	// importModel("bob_lamp/bob_lamp_update.fbx", Vector3(0,0,-5), 1.0f);
	// gameObject = importModel2("Avocado/glTF/Avocado.gltf", Vector3(150,0,0), 1000.0f, 0);
	importModel2("Avocado/glTF/Avocado.gltf", Vector3(150,0,190), 1000.0f, 0);
	importModel("Sponza/glTF/Sponza.gltf", Vector3(0,0,0), 1.0f, 0);
	importModel("CesiumMan/glTF/CesiumMan.gltf", Vector3(200,60,0), 20.0f, 0);
	importModel("BrainStem/glTF/BrainStem.gltf", Vector3(0,60,0), 20.0f, 0);

	// auto obj = importModel2("DamagedHelmet/glTF/DamagedHelmet.gltf", Vector3(100,100,0), 20.0f, 0);
    // mGameObjectsArray.push_back(obj);
	// importModel2("Fox/glTF/Fox.gltf", Vector3(300,0,0), 10.0f, 0);
	// gameObject = importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(-500,0,0), 20.0f, 0);
	// auto obj = importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(0,-50,0), 20.0f, 0);
    // mGameObjectsArray.push_back(obj);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(-300,0,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(0,0,0), 20.0f, 0);
    // importModel2("Vanguard-gltf/Vanguard.gltf", Vector3(-300,0,-120), 20.0f, 0);
    importModel2("Rumba/Rumba.gltf", Vector3(-300,0,-200), 1, 0);
    importModel2("Run/Run.gltf", Vector3(300,0,-200), 1, 0);
	// importModel2("Vanguard-gltf/Vanguard.gltf", Vector3(0,0,-300), 1, 0);

    size = 1;
    for(i32 x = -size; x < size; ++x)
	{
        // auto obj = importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(x*150,0,0), 20.0f, 0);
        // mGameObjectsArray.push_back(obj);

		for(i32 y = -size; y < size; ++y)
		{
			for(i32 z = -size; z < size; ++z)
            {
                
            }
		}
	}
    
    // Vector3 pos(0,0,0);
    // FOR_RANGE(x, -10, 10)
    // {
    //     FOR_RANGE(y, -10, 10)
    //     {
    //         FOR_RANGE(z, -10, 10)
    //         {
    //             importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(x,y,z) * 100, 20.0f*z, 0);
    //         }
    //     }
    // }

	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(150,0,60), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(300,0,60), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(-150,0,60), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(-300,0,60), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(0,0,60), 20.0f, 0);

	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(150,0,-60), 20.0f, 0);
	// // importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(300,0,-60), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(-150,0,-60), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(-300,0,-60), 20.0f, 0);
	// importModel2("BrainStem/glTF/BrainStem2.gltf", Vector3(0,0,-60), 60.0f, 0);

	// importModel2("bob_lamp/bob_lamp_update.fbx", Vector3(300,0,0), 20.0f, 0);
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
	// setPosition(Vector2(0,0)).
	setPosition(Vector2(-1,1)).
	setAdjustSizeToText(true).
	setSize(Vector2(0.5, 0.05f));

    // uiBuilder. 
	// setText("cApgfy").
	// create<UIText>().
	// getUIElement<UIText>();

    // uiBuilder.
	// setText("A").
	// create<UIButton>().
	// getUIElement<UIButton>()->
	// setOnPressedCallback([&, this](UIElement *uiElement){
	// });  
    
    uiBuilder.
	setText("A").
	create<UIText>();

    fpsCounter = uiBuilder.
	setText("100").
	create<UIText>().
    getUIElement<UIText>();

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
	setText("Edit").
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

    uiBuilder.
	setText("a").
	create<UIEditableText>();

	uiBuilder.restoreAll();

	cameraGameObject = GET_SYSTEM(ScenesManager).getCurrentScene()->getCameraGameObject();
}

void Sandbox::update()
{
	PROFILER_CPU()

	Vector2 currentMousePosition = GET_SYSTEM(Input).getMousePosition();




	rotation += 1.0f;


	


	



	y += 30 * GET_SYSTEM(Time).getDeltaTimeSeconds();

    PROFILER_BLOCK_CPU("Sandbox Camera");
	Transform* cameraTransform = &cameraGameObject->mTransform.get();
	Matrix4 cameraRotationMatrix = cameraGameObject->mTransform->getLocalRotationMatrix();
	cameraRotationMatrix.invert();

	f32 speed = 200 * GET_SYSTEM(Time).getDeltaTimeSeconds();

    //gameObject->mTransform->addTranslation(Vector3(0,0,-0.5));

	if(GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_LEFT))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Vector3(-speed,0,0)));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_RIGHT))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Vector3(speed,0,0)));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_UP))
	{
		cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Vector3(0,0,-speed)));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_DOWN))
	{
		cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Vector3(0,0,speed)));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_PAGE_UP))
	{
		cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Vector3(0,speed,0)));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_PAGE_DOWN))
	{
		cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Vector3(0,-speed,0)));
	}

	if(!mousePosition.eq(currentMousePosition))
	{
		Vector2 mouseVector = (currentMousePosition - mousePosition).nor() * speed;
		Vector3 direction;

		f32 yaw = mouseVector.x;
		f32 pitch = mouseVector.y;

		cameraTransform->addLocalRotation(Vector3(pitch, -yaw, 0));
	}

	mousePosition = currentMousePosition;

    FOR_ARRAY(i, mGameObjectsArray)
    {
        Ptr<Transform> transform = mGameObjectsArray[i]->mTransform;
        transform->addLocalRotation(Vector3(speed/30.0f, speed/30.0f, speed/30.0f));
    }

    PROFILER_END_BLOCK();

    // PROFILER_BLOCK_CPU("Draw Sandbox Lines");
    // // -x to x
	// GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(-1000,0,0), Vector3(1000,0,0)), 2, true, Vector4(1,0,0,1));

    // // x
	// GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(1000,0,0), Vector3(1000,100,0)), 1, true, Vector4(1,0,0,1));
    // // -x
	// GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(-1000,0,0), Vector3(-1000,50,0)), 1, true, Vector4(1,1,0,1));

    // // -y to y
	// GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,-1000,0), Vector3(0,1000,0)), 2, true, Vector4(0,1,0,1));

    // // -z to z
	// GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,0,-1000), Vector3(0,0,1000)), 2, true, Vector4(0,0,1,1));

    // // z
	// GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,0,1000), Vector3(0,100,1000)), 1, true, Vector4(0,0,1,1));
    // // -z
	// GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(0,0,-1000), Vector3(0,50,-1000)), 1, true, Vector4(0,1,1,1));

	for(i32 x = -1000; x < 1000; x+=50)
	{
		GET_SYSTEM(RenderEngine).drawLine(Line(Vector3(x,0,-1000), Vector3(x,0,1000)), 1, true, Vector4(0,0,1,0.3f));
	}
    // PROFILER_END_BLOCK();

    f32 fps = 1000.0f/GET_SYSTEM(Time).getDeltaTimeMillis();
    // fpsCounter->setText(std::to_string((u32)fps));
}

void Sandbox::terminate()
{
	
}

Ptr<GameObject> Sandbox::createSprite(const Vector3& v, f32 size)
{
	Ptr<GameObject> gameObject = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<GameObject>();
	gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Vector3(size,size,size));

    RendererData rendererData;
	rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Cube>();

    MaterialData materialData;
    materialData.mTexturePaths[(u32)TextureType::BASE_COLOR] = "resources/snorlax-fill.png";
	rendererData.mMaterial = (GET_SYSTEM(MaterialManager).createMaterial(materialData));

	gameObject->createComponent<MeshRenderer>(rendererData);

	return gameObject;
}

Ptr<GameObject> Sandbox::importModel( const std::string& pFile, const Vector3& v, f32 size, f32 rot)
{
	Ptr<const Model> model = GET_SYSTEM(ModelManager).loadModel(pFile);

    Ptr<GameObject> gameObject = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<GameObject>();
	gameObject->mIsStatic = true;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Vector3(1,1,1) * size);
	gameObject->mTransform->setLocalRotation(Vector3(0,rot,0));

    ModelRendererData modelRendererData;
    modelRendererData.mModel = model;
    modelRendererData.mIsInstanced = true;

	gameObject->createComponent<ModelRenderer>(modelRendererData);
    return gameObject;
}

Ptr<GameObject> Sandbox::importModel2( const std::string& pFile, const Vector3& v, f32 size, f32 rot)
{
	Ptr<const Model> model = GET_SYSTEM(ModelManager).loadModel(pFile);

	Ptr<GameObject> gameObject = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<GameObject>();
	gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Vector3(1,1,1) * size);
	gameObject->mTransform->setLocalRotation(Vector3(0,rot,0));

    ModelRendererData modelRendererData;
    modelRendererData.mModel = model;
    modelRendererData.mIsInstanced = true;
	gameObject->createComponent<ModelRenderer>(modelRendererData);

    GPULight data;
    data.mPosition = v + Vector3(0,500,0);
    data.mAmbientIntensity = 0.5f;
    data.mSpecularIntensity = 1.8f;

	gameObject->createComponent<Light>(data);
    return gameObject;
}

void Sandbox::handlePressedKeys()
{
	if(GET_SYSTEM(Input).isKeyPressedOnce(GLFW_KEY_KP_ADD))
	{
	}

	if(GET_SYSTEM(Input).isKeyPressedOnce(GLFW_KEY_KP_SUBTRACT))
	{
	}
}
