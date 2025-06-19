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

	mCameraSceneObject = GET_SYSTEM(ScenesManager).getCameraSceneObject();
	mCameraSceneObject->mTransform->setLocalPosition(Vector3(0,0,100));
    TComponentPtr<Camera> camera = mCameraSceneObject->getFirstComponent<Camera>();
    Vector2 windowSize = GET_SYSTEM(WindowManager).getMainWindow()->getWindowSize();
    // camera->setOrtho(-windowSize.x, windowSize.x, -windowSize.y, windowSize.y, -1000, 1000);

    // createPointLight(Vector3(0,50,0), 20);

    mDirectionalLight = createDirectionalLight(Vector3(0,2,0), Vector3::smForward + -Vector3::smUp);
    createSprite(Vector3(0,0,0), 100);
    // createSprite(Vector3(-100,0,0), 100);
    // createSprite(Vector3(100,0,0), 100);
    // createSprite(Vector3(0,0,-100), 10);

    // importModel("bob_lamp/bob_lamp_update.fbx", Vector3(0,0,-5), 1.0f);
	// sceneObject = importModel2("Avocado/glTF/Avocado.gltf", Vector3(150,0,0), 1000.0f, 0);
	// importModel("Floor/Floor.gltf", Vector3(0,0,0), 1.0f, Vector3(0,0,0), true);
	// importModel("Wall/Wall.gltf", Vector3(0,0,0), 1000.0f, Vector3(0,0,0), true);
	// importModel("Wall/Wall.gltf", Vector3(0,0,1000), 1.0f, Vector3(0,0,0), true);
	// importModel("BoxTextured/glTF//BoxTextured.gltf", Vector3(0,0,0), 100.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Vector3(150,0,0), 1000.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Vector3(0,-5,70), 1000.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Vector3(0,-5,-70), 1000.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avvocado.gltf", Vector3(-300,-5,0), 1000.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Vector3(150,-5,0), 1000.0f, Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Vector3(-150,-5,0), 1000.0f, Vector3(0,0,0), true);
	// importModel("Bistro/Bistro.gltf", Vector3(0,0,0), 1.0f, 0, true);
	// importModel("Sponza/new/Sponza.gltf", Vector3(0,0,0), 100.0f, Vector3(0,0,0), true);
	// importModel("Building_1/building1.gltf", Vector3(50,0,0), 10.0f, Vector3(0,0,0), true);
	// importModel("Building_1/building1.gltf", Vector3(-50,0,50), 10.0f, Vector3(0,0,0), true);
	// importModel("building/building.gltf", Vector3(0,0,0), 10.0f, Vector3(0,0,0), true);
	// importModel("building/building.gltf", Vector3(-50,0,0), 10.0f, Vector3(0,0,0), true);
	// importModel("s5avfuixwjy8-calisma/Calisma2.gltf", Vector3(0,0,0), 100.0f, Vector3(0,0,0), true);
	// importModel("mountain/mount.blend1.gltf", Vector3(0,0,0), 500.0f, Vector3(0,0,0), true);
	// importModel("CesiumMan/glTF/CesiumMan.gltf", Vector3(0,0,0), 100.0f, Vector3(90,0,0), false);
	// importModel("CesiumMan/glTF/CesiumMan.gltf", Vector3(100,0,0), 100.0f, Vector3(90,0,0), false);
	// importModel("BrainStem/glTF/BrainStem.gltf", Vector3(0,0,0), 100.0f, Vector3(90,0,0), false);
    // mSceneObjectsArray.push_back(obj);
	
	importModel("CesiumMan/glTF/CesiumMan.gltf", Vector3(0,0,0), 100.0f, Vector3(90,0,0), false);
    i32 size = 12;            
    FOR_RANGE(i, -size, size)
    {
        FOR_RANGE(j, -size, size)
        {
			// importModel("BrainStem/glTF/BrainStem.gltf", Vector3(150*i,0,150*j), 100.0f, Vector3(0,0,0), true);
            // if(j % 2 == 0)
            {
				// importModel("tower/tower.gltf", Vector3(500*i,3,500*j), 100.0f, Vector3(0,0,0), false);
				// importModel("CesiumMan/glTF/CesiumMan.gltf", Vector3(500*i,650,500*j), 100.0f, Vector3(90,0,0), false);
            }
            // else
            // {
			// 	importModel("cottage/cottage_blender.gltf", Vector3(250*i,0,250*j), 100.0f, Vector3(0,0,0), true);
            // }
        }
    }


	// obj = importModel("DamagedHelmet/glTF/DamagedHelmet.gltf", Vector3(0,270,0), 100.0f, Vector3(0,180,180), false);
    // mSceneObjectsArray.push_back(obj);
	// importModel("Fox/glTF/Fox.gltf", Vector3(300,0,0), 10.0f, Vector3(0,0,0), true);
	// importModel2("BrainStem/glTF/BrainStem.gltf", Vector3(0,0,0), 20.0f, 0);
	// importModel("bob_lamp/bob_lamp_update.gltf", Vector3(0,0,0), 20.0f, Vector3(0,0,0), true);
	// auto obj = importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(0,-50,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(-300,0,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Vector3(0,0,0), 20.0f, 0);

    createUI();
    // mousePick();

    // mUISceneTree = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UISceneTree>();
    // mUISceneTree->mTransform->setLocalPosition(Vector2(-0.9, 0.8));
    // mUISceneTree->update();
}

void Editor::update()
{
	PROFILER_CPU()

    TComponentPtr<Camera> camera = mCameraSceneObject->getFirstComponent<Camera>();
	TComponentPtr<Transform> cameraTransform = mCameraSceneObject->mTransform;
	f32 speed = 400 * GET_SYSTEM(Time).getDeltaTimeSeconds();

	Matrix4 cameraRotationMatrix = mCameraSceneObject->mTransform->getLocalRotationMatrix();
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
    if(mSelectedSceneObject)
    {
        Vector3 position = camera->getGPUCamera().screenToWorld(currentMousePosition, mSelectedSceneObject->mTransform->getWorldPosition().z);
        // position.z = mSelectedSceneObject->mTransform->getLocalPosition().z;
        mSelectedSceneObject->mTransform->setLocalPosition(position);
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

	// LOG_VAR(cameraTransform->getLocalPosition().x)
	// LOG_VAR(cameraTransform->getLocalPosition().y)
	// LOG_VAR(cameraTransform->getLocalPosition().z)

	mLastMousePosition = currentMousePosition;

    //mDirectionalLight->mTransform->addLocalRotation(Vector3(0,0.1f,0));

    // PROFILER_CPU_NAMED("Draw Editor Lines");
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

    f32 fps = 1000.0f/GET_SYSTEM(Time).getDeltaTimeMillis();
    // LOG_VAR(fps)
    if(mFPSCounter)
    {
        mFPSCounter->setText(HashedString(std::to_string((u32)fps)));
    }

    mousePick();

	if(mAxisViewer)
	{
		mAxisViewer->update();
	}
    // mUISceneTree->update();
    // mUITransform->update(cameraTransform);

}

void Editor::terminate()
{

}

EntityPtr Editor::createSprite(const Vector3& v, f32 size)
{
	TEntityPtr<SceneObject> sceneObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createSceneObject<SceneObject>();
	// sceneObject->mIsStatic = false;
	// sceneObject->mTransform->setLocalPosition(v);
	// sceneObject->mTransform->setLocalScale(Vector3(size,size,size));

    // RendererData rendererData;
	// rendererData.mMesh = GET_SYSTEM(GPUMeshFactory).getPrimitive<Rectangle>();

    GPUShaderData shaderData;
    shaderData.mGPUShaderTextureBindings.mTextureBindings.insert_or_assign(TextureBindingNames::smBaseColor, TextureBinding{"resources/snorlax-fill.png"});
	PropertiesBlockGPUShaderDefault shaderPropertiesBlock;
	// rendererData.mShader = (GET_SYSTEM(GPUShaderManager).createShader<GPUShaderDefault>(shaderData));

	// TComponentPtr<MeshRenderer> renderer = EntityComponentManager::getInstance().requestComponent<MeshRenderer>();
    // renderer->init(rendererData);
	// sceneObject->addComponent(renderer);

	GPURenderItemData rendererData;
    rendererData.mMesh = GET_SYSTEM(GPUMeshFactory).getPrimitive<Rectangle>();
    rendererData.mShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderDefault, PropertiesBlockGPUShaderDefault>(shaderData, shaderPropertiesBlock);
    rendererData.mRenderPassIDs = {
        ClassManager::getClassMetadata<RenderPassGeometry>().mClassDefinition.getId(),
    };

	TComponentPtr<MeshRenderer> renderer = EntityComponentManager::getInstance().requestComponent<MeshRenderer>();
	renderer->init(rendererData);
	sceneObject->addComponent(renderer);

	return sceneObject;
}

EntityPtr Editor::createPointLight(const Vector3& v, f32 size)
{
	TEntityPtr<SceneObject> sceneObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createSceneObject<SceneObject>();
	sceneObject->mIsStatic = false;
	sceneObject->mTransform->setLocalPosition(v);
	sceneObject->mTransform->setLocalScale(Vector3(size,size,size));

    PointLightData data;
    data.mPosition = v;
    data.mDiffuse = Vector3(1,1,1) * 250000;

	TComponentPtr<PointLight> pointLight = EntityComponentManager::getInstance().requestComponent<PointLight>();
    pointLight->init(data);
	sceneObject->addComponent(pointLight);

	return sceneObject;
}

EntityPtr Editor::createDirectionalLight(const Vector3& v, const Vector3& dir)
{
	TEntityPtr<SceneObject> sceneObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createSceneObject<SceneObject>();
    sceneObject->mIsStatic = false;
	sceneObject->mTransform->setLocalPosition(v);
	sceneObject->mTransform->lookAt(v + dir);

    DirectionalLightData directionalLightData;
    directionalLightData.mDirection = dir;
    directionalLightData.mDiffuse = Vector3(0.65,0.2,0.1) * 20;

	TComponentPtr<DirectionalLight> dirLight = EntityComponentManager::getInstance().requestComponent<DirectionalLight>();
    dirLight->init(directionalLightData);
	sceneObject->addComponent(dirLight);

	return sceneObject;
}

EntityPtr Editor::mousePick()
{

    f32 speed = 100 * GET_SYSTEM(Time).getDeltaTimeSeconds();
    EntityPtr obj;
    FOR_LIST(it, mSceneObjectsArray)
    {
        (*it)->mTransform->addLocalRotation(Vector3(0,0.1f,0));
        // const Cube& bbox = (*it)->getFirstComponent<MeshRenderer>()->getOcTreeBoundingBox();
        // TComponentPtr<Camera> camera = mCameraSceneObject->getFirstComponent<Camera>();
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

EntityPtr Editor::importModel( const std::string& pFile, const Vector3& v, f32 size, const Vector3& rot, bool isStatic)
{
	WeakPtr<const Model> model = GET_SYSTEM(ModelManager).loadModel(pFile);

    TEntityPtr<SceneObject> sceneObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createSceneObject<SceneObject>();
	sceneObject->mIsStatic = isStatic;
	sceneObject->mTransform->setLocalPosition(v);
	sceneObject->mTransform->setLocalScale(Vector3::smOne * size);
	sceneObject->mTransform->setLocalRotation(rot);

    ModelRendererData modelRendererData;
    modelRendererData.mModel = model;
    modelRendererData.mRenderPassIDs =
    {
        ClassManager::getClassMetadata<RenderPassGeometry>().mClassDefinition.getId(),
        ClassManager::getClassMetadata<RenderPassShadowMap>().mClassDefinition.getId()
    };

	TComponentPtr<ModelRenderer> modelRenderer = EntityComponentManager::getInstance().requestComponent<ModelRenderer>();
    modelRenderer->init(modelRendererData);
	sceneObject->addComponent(modelRenderer);
    return sceneObject;
}

void Editor::handlePressedKeys()
{

}

void Editor::handleMouse()
{
	if(GET_SYSTEM(Input).isMouseButtonPressedOnce(GLFW_MOUSE_BUTTON_LEFT))
	{
        // GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mBuildings.front());
        // mBuildings.pop_front();
        // TComponentPtr<Camera> camera = mCameraSceneObject->getFirstComponent<Camera>();
        // Vector2 currentMousePosition = GET_SYSTEM(Input).getMousePosition();
        // Vector3 position = camera->screenToWorld(currentMousePosition, 0);
        // auto obj = importModel("DamagedHelmet/glTF/DamagedHelmet.gltf", position, 1.0f, Vector3(0,180,180), false);
        // mSceneObjectsArray.push_back(obj);
        if(!mSelectedSceneObject)
        {
            mSelectedSceneObject = mousePick();
        }
	}
    else
    {
        mSelectedSceneObject.reset();
    }
    
    if(GET_SYSTEM(Input).isMouseButtonPressedOnce(GLFW_MOUSE_BUTTON_RIGHT))
    {
        // mBuildings.push_back(importModel("Building_1/building1.gltf", Vector3(0,100,0), 10.0f, Vector3(0,0,0), false));
        
    }
}

void Editor::createUI()
{
    mAxisViewer = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UIAxisGizmo>();
    mAxisViewer->mTransform->setLocalPosition(Vector2(-0.9, -0.8));
    mAxisViewer->createAxis();

    UIBuilder uiBuilder;

	uiBuilder.
	// setPosition(Vector2(0,0)).
	setPosition(Vector2(-1,1)).
	setAdjustSizeToText(true).
	setSize(Vector2(0.5, 0.05f)).
    setTextScale(0.5f);

    // uiBuilder.
	// setText("cApgfy").
	// create<UIText>().
	// getUIElement<UIText>();

    // uiBuilder.
	// setText("Asdc hghjyYRTL").
	// create<UIButton>().
	// getUIElement<UIButton>()->
	// setOnPressedCallback([&, this](UIElement *uiElement){
	// });


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

    uiBuilder.
	// setPosition(Vector2(0,0)).
	setPosition(Vector2(-1,1)).
	// setAdjustSizeToText(true).
	setSize(Vector2(0.5, 0.5f));

    // uiBuilder.
	// create<UIList>().
	// getUIElement<UIList>()->
	// addOption("A", [&](UIElement *uiElement)
	// {

	// }).
    // toggle();

    // mUITransform = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UITransform>();
    // mUITransform->mTransform->setLocalPosition(Vector2(-0.7, -0.8));
    // mUITransform->mIsStatic = true;
    // mUIVector->update();
}
