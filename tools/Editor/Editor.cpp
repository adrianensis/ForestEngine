#include "Editor.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"

void Editor::init()
{
    PROFILER_CPU();
	SUBSCRIBE_TO_EVENT(InputEventKeyPressed, nullptr, this, [&](const Event *event)
	{
		handlePressedKeys();
	});
    
    SUBSCRIBE_TO_EVENT(InputEventMouseButtonHold, nullptr, this, [&](const Event *event)
	{
		handleMouse();
	});    

    SUBSCRIBE_TO_EVENT(InputEventMouseButtonReleased, nullptr, this, [&](const Event *event)
	{
		handleMouse();
	});
}

void Editor::firstUpdate()
{
    PROFILER_CPU();
    
	mCameraGameObject = mGameObject->mScene->getCameraGameObject();
	// mCameraGameObject->mTransform->setLocalPosition(Vector3::smZero);
    Ptr<Camera> camera = mCameraGameObject->getFirstComponent<Camera>();
    Vector2 windowSize = GET_SYSTEM(Window).getWindowSize();
    // camera->setOrtho(-windowSize.x, windowSize.x, -windowSize.y, windowSize.y, -1000, 1000);

    createPointLight(Vector3(0,300,0), 20);
    createDirectionalLight();
    // createSprite(Vector3(-100,0,0), 100);
    // createSprite(Vector3(100,0,0), 100);
    // createSprite(Vector3(0,0,-100), 10);

    // importModel("bob_lamp/bob_lamp_update.fbx", Vector3(0,0,-5), 1.0f);
	// gameObject = importModel2("Avocado/glTF/Avocado.gltf", Vector3(150,0,0), 1000.0f, 0);
	// importModel2("Avocado/glTF/Avocado.gltf", Vector3(150,0,190), 1000.0f, 0);
	importModel("Sponza/glTF/Sponza.gltf", Vector3(0,0,0), 1.0f, 0);
	// importModel("CesiumMan/glTF/CesiumMan.gltf", Vector3(0,60,0), 20.0f, 0);

	auto obj = importModel2("DamagedHelmet/glTF/DamagedHelmet.gltf", Vector3(20,150,0), 20.0f, 0);
    // mGameObjectsArray.push_back(obj);
	// importModel2("Fox/glTF/Fox.gltf", Vector3(300,0,0), 10.0f, 0);
	// importModel2("BrainStem/glTF/BrainStem.gltf", Vector3(0,0,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(0,0,0), 20.0f, 0);
	// auto obj = importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(0,-50,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(-300,0,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(0,0,0), 20.0f, 0);

    createUI();
    mousePick();
}

void Editor::update()
{
	PROFILER_CPU()

    Ptr<Camera> camera = mCameraGameObject->getFirstComponent<Camera>();
	Transform* cameraTransform = &mCameraGameObject->mTransform.get();
	f32 speed = 200 * GET_SYSTEM(Time).getDeltaTimeSeconds();

    //gameObject->mTransform->addTranslation(Vector3(0,0,-0.5));

	if(GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_LEFT))
	{
        cameraTransform->addLocalTranslation(Vector3(-speed,0,0));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_RIGHT))
	{
        cameraTransform->addLocalTranslation(Vector3(speed,0,0));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_UP))
	{
		cameraTransform->addLocalTranslation(Vector3(0,0,-speed));
		// cameraTransform->addLocalTranslation(Vector3(0,speed,0));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_DOWN))
	{
		cameraTransform->addLocalTranslation(Vector3(0,0,speed));
		// cameraTransform->addLocalTranslation(Vector3(0,-speed,0));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_PAGE_UP))
	{
		cameraTransform->addLocalTranslation(Vector3(0,speed,0));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_PAGE_DOWN))
	{
		cameraTransform->addLocalTranslation(Vector3(0,-speed,0));
	}
    else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_HOME))
	{
		cameraTransform->addLocalRotation(Vector3(0,-speed/10,0));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_END))
	{
		cameraTransform->addLocalRotation(Vector3(0,speed/10,0));
	}

    Vector2 currentMousePosition = GET_SYSTEM(Input).getMousePosition();
    // currentMousePosition.set(-1,0,0);
    // LOG_VAR(currentMousePosition.x);
    // LOG_VAR(currentMousePosition.y);
    if(mSelectedGameObject)
    {
        Vector3 position = camera->screenToWorld(currentMousePosition, mSelectedGameObject->mTransform->getWorldPosition().z);
        // position.z = mSelectedGameObject->mTransform->getLocalPosition().z;
        mSelectedGameObject->mTransform->setLocalPosition(position);
        // LOG_VAR(position.x);
        // LOG_VAR(position.y);
        // LOG_VAR(position.z);
    }

	// if(!mousePosition.eq(currentMousePosition))
	// {
	// 	Vector2 mouseVector = (currentMousePosition - mousePosition).nor() * speed;
	// 	Vector3 direction;

	// 	f32 yaw = mouseVector.x;
	// 	f32 pitch = mouseVector.y;

	// 	cameraTransform->addLocalRotation(Vector3(pitch, -yaw, 0));
	// }

	// mousePosition = currentMousePosition;

    // PROFILER_BLOCK_CPU("Draw Editor Lines");
    // // -x to x
	// GET_SYSTEM(DebugRenderer).drawLine(Line(Vector3(-1000,0,0), Vector3(1000,0,0)), 2, true, Vector4(1,0,0,1));

    // // x
	// GET_SYSTEM(DebugRenderer).drawLine(Line(Vector3(1000,0,0), Vector3(1000,100,0)), 1, true, Vector4(1,0,0,1));
    // // -x
	// GET_SYSTEM(DebugRenderer).drawLine(Line(Vector3(-1000,0,0), Vector3(-1000,50,0)), 1, true, Vector4(1,1,0,1));

    // // -y to y
	// GET_SYSTEM(DebugRenderer).drawLine(Line(Vector3(0,-1000,0), Vector3(0,1000,0)), 2, true, Vector4(0,1,0,1));

    // // -z to z
	// GET_SYSTEM(DebugRenderer).drawLine(Line(Vector3(0,0,-1000), Vector3(0,0,1000)), 2, true, Vector4(0,0,1,1));

    // // z
	// GET_SYSTEM(DebugRenderer).drawLine(Line(Vector3(0,0,1000), Vector3(0,100,1000)), 1, true, Vector4(0,0,1,1));
    // // -z
	// GET_SYSTEM(DebugRenderer).drawLine(Line(Vector3(0,0,-1000), Vector3(0,50,-1000)), 1, true, Vector4(0,1,1,1));

	for(i32 x = -1000; x < 1000; x+=50)
	{
		GET_SYSTEM(DebugRenderer).drawLine(Line(Vector3(x,0,-1000), Vector3(x,0,1000)), 1, true, Vector4(0,0,1,0.3f));
	}
    // PROFILER_END_BLOCK();

    f32 fps = 1000.0f/GET_SYSTEM(Time).getDeltaTimeMillis();
    if(fpsCounter)
    {
        fpsCounter->setText(std::to_string((u32)fps));
    }

    mousePick();
}

void Editor::terminate()
{
	
}

Ptr<GameObject> Editor::createSprite(const Vector3& v, f32 size)
{
	Ptr<GameObject> gameObject = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<GameObject>();
	gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Vector3(size,size,size));

    RendererData rendererData;
    rendererData.mIsInstanced = true;
	rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();

    MaterialData materialData;
	materialData.mReceiveLight = false;
    materialData.mTextureBindings[(u32)TextureMap::BASE_COLOR] = MaterialTextureBinding{"resources/snorlax-fill.png", GPUPipelineStage::FRAGMENT};
	rendererData.mMaterial = (GET_SYSTEM(MaterialManager).createMaterial<MaterialRuntimeDefault>(materialData));

	gameObject->createComponent<MeshRenderer>(rendererData);

    mGameObjectsArray.push_back(gameObject);

	return gameObject;
}

Ptr<GameObject> Editor::createPointLight(const Vector3& v, f32 size)
{
	Ptr<GameObject> gameObject = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<GameObject>();
	gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Vector3(size,size,size));

    PointLightData data;
    data.mPosition = v;
    data.mDiffuse = Vector3(1,1,1) * 250000;

	gameObject->createComponent<PointLight>(data);

	return gameObject;
}

Ptr<GameObject> Editor::createDirectionalLight()
{
	Ptr<GameObject> gameObject = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<GameObject>();
    
    DirectionalLightData directionalLightData;
    directionalLightData.mDiffuse = Vector3(0.65,0.2,0.1) * 20;

	gameObject->createComponent<DirectionalLight>(directionalLightData);

	return gameObject;
}

Ptr<GameObject> Editor::mousePick()
{
    Ptr<GameObject> obj;
    FOR_LIST(it, mGameObjectsArray)
    {
        const Cube& bbox = (*it)->getFirstComponent<MeshRenderer>()->getOcTreeBoundingBox();
        Ptr<Camera> camera = mCameraGameObject->getFirstComponent<Camera>();
        Cube bboxScreenSpace(
            camera->worldToScreen(bbox.getLeftTopFront()),
            camera->worldToScreen(bbox.getLeftTopFront() + bbox.getSize()) - camera->worldToScreen(bbox.getLeftTopFront())
        );

        // GET_SYSTEM(DebugRenderer).drawCube(bboxScreenSpace, 1, false, Vector4(0.3,0,1,1));

        Vector3 mousePosition = GET_SYSTEM(Input).getMousePosition();
        bool hit = Geometry::testCubePoint(bboxScreenSpace, mousePosition, 0);
        if(hit)
        {
            Cube hitMarker = Cube(mousePosition, Vector3(0.01,0.01,0.01));
            GET_SYSTEM(DebugRenderer).drawCube(hitMarker, 1, false, Vector4(1,0,0,1));
            GET_SYSTEM(DebugRenderer).drawCube(bboxScreenSpace, 1, false, Vector4(1,0,0,1));

            obj = *it;
        }
    }

    return obj;
}

Ptr<GameObject> Editor::importModel( const std::string& pFile, const Vector3& v, f32 size, f32 rot)
{
	Ptr<const Model> model = GET_SYSTEM(ModelManager).loadModel(pFile);

    Ptr<GameObject> gameObject = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<GameObject>();
	gameObject->mIsStatic = true;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Vector3::smOne * size);
	gameObject->mTransform->setLocalRotation(Vector3(0,rot,0));

    ModelRendererData modelRendererData;
    modelRendererData.mModel = model;
    modelRendererData.mIsInstanced = true;

	gameObject->createComponent<ModelRenderer>(modelRendererData);
    return gameObject;
}

Ptr<GameObject> Editor::importModel2( const std::string& pFile, const Vector3& v, f32 size, f32 rot)
{
	Ptr<const Model> model = GET_SYSTEM(ModelManager).loadModel(pFile);

	Ptr<GameObject> gameObject = GET_SYSTEM(ScenesManager).getCurrentScene()->createGameObject<GameObject>();
	gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Vector3::smOne * size);
	gameObject->mTransform->setLocalRotation(Vector3(0,rot,0));

    ModelRendererData modelRendererData;
    modelRendererData.mModel = model;
    modelRendererData.mIsInstanced = true;
	gameObject->createComponent<ModelRenderer>(modelRendererData);

    return gameObject;
}

void Editor::handlePressedKeys()
{

}

void Editor::handleMouse()
{
	if(GET_SYSTEM(Input).isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
	{
        if(!mSelectedGameObject)
        {
            mSelectedGameObject = mousePick();
        }
	}
    else
    {
        mSelectedGameObject.invalidate();
    }
}

void Editor::createUI()
{
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

    uiBuilder.
	setText("Asdc hghjyYRTL").
	create<UIButton>().
	getUIElement<UIButton>()->
	setOnPressedCallback([&, this](UIElement *uiElement){
	});  
    

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
}
