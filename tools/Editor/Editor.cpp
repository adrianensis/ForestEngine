#include "Editor.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Core/Input/Input.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Core/Time/TimeUtils.hpp"

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

	mCameraGameObject = GET_SYSTEM(ScenesManager).getCameraGameObject();
	// mCameraGameObject->mTransform->setLocalPosition(Vector3::smZero);
    TypedComponentHandler<Camera> camera = mCameraGameObject->getFirstComponent<Camera>();
    Vector2 windowSize = GET_SYSTEM(WindowManager).getMainWindow()->getWindowSize();
    // camera->setOrtho(-windowSize.x, windowSize.x, -windowSize.y, windowSize.y, -1000, 1000);

    mAxisViewer = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UIAxisGizmo>();
    mAxisViewer->mTransform->setLocalPosition(Vector2(-0.9, -0.8));
    mAxisViewer->createAxis();

    // createPointLight(Vector3(0,50,0), 20);

    mDirectionalLight = createDirectionalLight(Vector3(0,2,0), Vector3::smForward + -Vector3::smUp);
    // createSprite(Vector3(0,1000,0), 10);
    // createSprite(Vector3(-100,0,0), 100);
    // createSprite(Vector3(100,0,0), 100);
    // createSprite(Vector3(0,0,-100), 10);

    // importModel("bob_lamp/bob_lamp_update.fbx", Vector3(0,0,-5), 1.0f);
	// gameObject = importModel2("Avocado/glTF/Avocado.gltf", Vector3(150,0,0), 1000.0f, 0);
	// importModel("Floor/Floor.gltf", Vector3(0,0,0), 1.0f, Vector3(0,0,0), true);
	// importModel("Wall/Wall.gltf", Vector3(500,0,0), 1.0f, Vector3(0,90,0), true);
	// importModel("Wall/Wall.gltf", Vector3(0,0,1000), 1.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Vector3(300,-5,0), 1000.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Vector3(0,-5,70), 1000.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Vector3(0,-5,-70), 1000.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avvocado.gltf", Vector3(-300,-5,0), 1000.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Vector3(150,-5,0), 1000.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Vector3(-150,-5,0), 1000.0f, Vector3(0,0,0), true);
	// importModel("Bistro/Bistro.gltf", Vector3(0,0,0), 1.0f, 0, true);
	// importModel("Sponza/glTF/Sponza.gltf", Vector3(0,0,0), 100.0f, Vector3(0,0,0), true);
	// importModel("Building_1/building1.gltf", Vector3(50,0,0), 10.0f, Vector3(0,0,0), true);
	// importModel("Building_1/building1.gltf", Vector3(-50,0,50), 10.0f, Vector3(0,0,0), true);
	// importModel("building/building.gltf", Vector3(0,0,0), 10.0f, Vector3(0,0,0), true);
	// importModel("building/building.gltf", Vector3(-50,0,0), 10.0f, Vector3(0,0,0), true);
	// importModel("s5avfuixwjy8-calisma/Calisma2.gltf", Vector3(0,0,0), 100.0f, Vector3(0,0,0), true);
	// importModel("mountain/mount.blend1.gltf", Vector3(0,0,0), 500.0f, Vector3(0,0,0), true);
	// auto obj = importModel("CesiumMan/glTF/CesiumMan.gltf", Vector3(300,150,-150), 100.0f, Vector3(90,0,0), false);
    // mGameObjectsArray.push_back(obj);

    i32 size = 25;            
    FOR_RANGE(i, -size, size)
    {
        FOR_RANGE(j, -size, size)
        {
            if(j % 2 == 0)
            {
                mBuildings.push_back(importModel("Building_1/building1.gltf", Vector3(50*i,0,50*j), 10.0f, Vector3(0,0,0), true));
            }
            else
            {
        	    mBuildings.push_back(importModel("building/building.gltf", Vector3(50*i,0,50*j), 10.0f, Vector3(0,0,0), true));
            }
        }
    }


	// obj = importModel("DamagedHelmet/glTF/DamagedHelmet.gltf", Vector3(0,270,0), 100.0f, Vector3(0,180,180), false);
    // mGameObjectsArray.push_back(obj);
	// importModel("Fox/glTF/Fox.gltf", Vector3(300,0,0), 10.0f, Vector3(0,0,0), true);
	// importModel2("BrainStem/glTF/BrainStem.gltf", Vector3(0,0,0), 20.0f, 0);
	// importModel("bob_lamp/bob_lamp_update.gltf", Vector3(0,0,0), 20.0f, Vector3(0,0,0), true);
	// auto obj = importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(0,-50,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(-300,0,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(0,0,0), 20.0f, 0);

    createUI();
    // mousePick();

    // mUISceneTree = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UISceneTree>();
    // mUISceneTree->mTransform->setLocalPosition(Vector2(-0.9, 0.8));
    // mUISceneTree->update();
}

void Editor::update()
{
	PROFILER_CPU()

    TypedComponentHandler<Camera> camera = mCameraGameObject->getFirstComponent<Camera>();
	TypedComponentHandler<Transform> cameraTransform = mCameraGameObject->mTransform;
	f32 speed = 400 * GET_SYSTEM(Time).getDeltaTimeSeconds();

	Matrix4 cameraRotationMatrix = mCameraGameObject->mTransform->getLocalRotationMatrix();
	cameraRotationMatrix.invert();

	if(GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_LEFT))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Vector4(-speed,0,0,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Vector3(0,-speed,0));
        // cameraTransform->addLocalTranslation(Vector3(-speed,0,0));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_RIGHT))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Vector4(speed,0,0,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Vector3(0,speed,0));
        // cameraTransform->addLocalTranslation(Vector3(speed,0,0));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_UP))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Vector4(0,0,-speed,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Vector4(0,0,-speed,1));
        // cameraTransform->addLocalTranslation(Vector3(0,0,-speed));
		// cameraTransform->addLocalTranslation(Vector3(0,speed,0));
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_DOWN))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Vector4(0,0,speed,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Vector4(0,0,speed,1));
        // cameraTransform->addLocalTranslation(Vector3(0,0,speed));
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
		// cameraTransform->addLocalRotation(Vector3(0,-speed,0));
        cameraTransform->lookAt(cameraTransform->getWorldPosition() + Vector3::smForward);
	}
	else if (GET_SYSTEM(Input).isKeyPressed(GLFW_KEY_END))
	{
		// cameraTransform->addLocalRotation(Vector3(0,speed,0));
        cameraTransform->lookAt(cameraTransform->getWorldPosition() + -Vector3::smForward);
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

	if(!mLastMousePosition.eq(currentMousePosition))
	{
        f32 camSpeed = 200 * GET_SYSTEM(Time).getDeltaTimeSeconds();
		Vector2 mouseVector = (currentMousePosition - mLastMousePosition).nor() * camSpeed;
		Vector3 direction;

		f32 yaw = mouseVector.x;
		f32 pitch = mouseVector.y;

		cameraTransform->addLocalRotation(Vector3(pitch, -yaw, 0));
		// mDirectionalLight->mTransform->addLocalRotation(Vector3(0, -yaw, 0));
	}

	mLastMousePosition = currentMousePosition;

    //mDirectionalLight->mTransform->addLocalRotation(Vector3(0,0.1f,0));

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

	for(i32 x = -2000; x < 2000; x+=100)
	{
		GET_SYSTEM(DebugRenderer).drawLine(Line(Vector3(x,0,-2000), Vector3(x,0,2000)), 1, GeometricSpace::WORLD, Vector4(1,1,1,0.3f));
	}
    // PROFILER_END_BLOCK();

    f32 fps = 1000.0f/GET_SYSTEM(Time).getDeltaTimeMillis();
    LOG_VAR(fps)
    if(mFPSCounter)
    {
        mFPSCounter->setText(HashedString(std::to_string((u32)fps)));
    }

    mousePick();

    mAxisViewer->update();
    // mUISceneTree->update();
    mUITransform->update(cameraTransform);

}

void Editor::terminate()
{

}

EntityHandler Editor::createSprite(const Vector3& v, f32 size)
{
	TypedEntityHandler<GameObject> gameObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createGameObject<GameObject>();
	gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Vector3(size,size,size));

    RendererData rendererData;
	rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();

    MaterialData materialData;
    materialData.mTextureBindings.insert_or_assign(TextureBindingNames::smBaseColor, TextureBinding{"resources/snorlax-fill.png", GPUPipelineStage::FRAGMENT});
	rendererData.mMaterial = (GET_SYSTEM(MaterialManager).createMaterial<ShaderDefault>(materialData));

	gameObject->createComponent<MeshRenderer>(rendererData);

	return gameObject;
}

EntityHandler Editor::createPointLight(const Vector3& v, f32 size)
{
	TypedEntityHandler<GameObject> gameObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createGameObject<GameObject>();
	gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Vector3(size,size,size));

    PointLightData data;
    data.mPosition = v;
    data.mDiffuse = Vector3(1,1,1) * 250000;

	gameObject->createComponent<PointLight>(data);

	return gameObject;
}

EntityHandler Editor::createDirectionalLight(const Vector3& v, const Vector3& dir)
{
	TypedEntityHandler<GameObject> gameObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createGameObject<GameObject>();
    gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->lookAt(v + dir);

    DirectionalLightData directionalLightData;
    directionalLightData.mDirection = dir;
    directionalLightData.mDiffuse = Vector3(0.65,0.2,0.1) * 20;

	gameObject->createComponent<DirectionalLight>(directionalLightData);

	return gameObject;
}

EntityHandler Editor::mousePick()
{

    f32 speed = 100 * GET_SYSTEM(Time).getDeltaTimeSeconds();
    EntityHandler obj;
    FOR_LIST(it, mGameObjectsArray)
    {
        (*it)->mTransform->addLocalRotation(Vector3(0,0.1f,0));
        // const Cube& bbox = (*it)->getFirstComponent<MeshRenderer>()->getOcTreeBoundingBox();
        // TypedComponentHandler<Camera> camera = mCameraGameObject->getFirstComponent<Camera>();
        // Cube bboxScreenSpace(
        //     camera->worldToScreen(bbox.getLeftTopFront()),
        //     camera->worldToScreen(bbox.getLeftTopFront() + bbox.getSize()) - camera->worldToScreen(bbox.getLeftTopFront())
        // );

        // // GET_SYSTEM(DebugRenderer).drawCube(bboxScreenSpace, 1, false, Vector4(0.3,0,1,1));

        // Vector3 mousePosition = GET_SYSTEM(Input).getMousePosition();
        // bool hit = Geometry::testCubePoint(bboxScreenSpace, mousePosition, 0);
        // if(hit)
        // {
        //     Cube hitMarker = Cube(mousePosition, Vector3(0.01,0.01,0.01));
        //     GET_SYSTEM(DebugRenderer).drawCube(hitMarker, 1, GeometricSpace::SCREEN, Vector4(1,0,0,1));
        //     GET_SYSTEM(DebugRenderer).drawCube(bboxScreenSpace, 1, GeometricSpace::SCREEN, Vector4(1,0,0,1));

        //     obj = *it;
        // }
    }

    return obj;
}

EntityHandler Editor::importModel( const std::string& pFile, const Vector3& v, f32 size, const Vector3& rot, bool isStatic)
{
	Ptr<const Model> model = GET_SYSTEM(ModelManager).loadModel(pFile);

    TypedEntityHandler<GameObject> gameObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createGameObject<GameObject>();
	gameObject->mIsStatic = isStatic;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Vector3::smOne * size);
	gameObject->mTransform->setLocalRotation(rot);

    ModelRendererData modelRendererData;
    modelRendererData.mModel = model;
    modelRendererData.mRenderPassIDs =
    {
        ClassManager::getClassMetadata<RenderPassGeometry>().mClassDefinition.getId(),
        ClassManager::getClassMetadata<RenderPassShadowMap>().mClassDefinition.getId()
    };

	gameObject->createComponent<ModelRenderer>(modelRendererData);
    return gameObject;
}

void Editor::handlePressedKeys()
{

}

void Editor::handleMouse()
{
	if(GET_SYSTEM(Input).isMouseButtonPressedOnce(GLFW_MOUSE_BUTTON_LEFT))
	{
        // GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(mBuildings.front());
        // mBuildings.pop_front();
        // TypedComponentHandler<Camera> camera = mCameraGameObject->getFirstComponent<Camera>();
        // Vector2 currentMousePosition = GET_SYSTEM(Input).getMousePosition();
        // Vector3 position = camera->screenToWorld(currentMousePosition, 0);
        // auto obj = importModel("DamagedHelmet/glTF/DamagedHelmet.gltf", position, 1.0f, Vector3(0,180,180), false);
        // mGameObjectsArray.push_back(obj);
        if(!mSelectedGameObject)
        {
            mSelectedGameObject = mousePick();
        }
	}
    else
    {
        mSelectedGameObject.reset();
    }
    
    if(GET_SYSTEM(Input).isMouseButtonPressedOnce(GLFW_MOUSE_BUTTON_RIGHT))
    {
        // mBuildings.push_back(importModel("Building_1/building1.gltf", Vector3(0,100,0), 10.0f, Vector3(0,0,0), false));
        
    }
}

void Editor::createUI()
{
    UIBuilder uiBuilder;

	uiBuilder.
	// setPosition(Vector2(0,0)).
	setPosition(Vector2(-1,1)).
	setAdjustSizeToText(true).
	setSize(Vector2(0.5, 0.05f)).
    setTextScale(0.5f);

    uiBuilder.
	setText("cApgfy").
	create<UIText>().
	getUIElement<UIText>();

    uiBuilder.
	setText("Asdc hghjyYRTL").
	create<UIButton>().
	getUIElement<UIButton>()->
	setOnPressedCallback([&, this](UIElement *uiElement){
	});


    mFPSCounter = uiBuilder.
	setText("000").
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

    // uiBuilder.
	// // setPosition(Vector2(0,0)).
	// setPosition(Vector2(-1,1)).
	// // setAdjustSizeToText(true).
	// setSize(Vector2(0.5, 0.5f));

    // uiBuilder.
	// create<UIList>().
	// getUIElement<UIList>()->
	// addOption("A", [&](UIElement *uiElement)
	// {

	// }).
    // toggle();

    mUITransform = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UITransform>();
    mUITransform->mTransform->setLocalPosition(Vector2(-0.7, -0.8));
    mUITransform->mIsStatic = true;
    // mUIVector->update();
}
