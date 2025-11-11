#include "Editor.hpp"
#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"
#include "GPU/Mesh/GPUMeshFactory.hpp"
#include "GPU/GPUInstance.hpp"
#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Graphics/Model/ModelRenderer.hpp"
#include "Scene/SceneObject.hpp"
#include "Window/WindowManager.hpp"

#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Engine/Input/Input.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Core/Time/TimeUtils.hpp"

void Editor::init()
{
    PROFILER_CPU();
	SUBSCRIBE_TO_EVENT(Input::InputEventKeyPressed, nullptr, this, [&](const Event::Event *event)
	{
		handlePressedKeys();
	});

    SUBSCRIBE_TO_EVENT(Input::InputEventMouseButtonHold, nullptr, this, [&](const Event::Event *event)
	{
		handleMouse();
	});

    SUBSCRIBE_TO_EVENT(Input::InputEventMouseButtonReleased, nullptr, this, [&](const Event::Event *event)
	{
		handleMouse();
	});
}

void Editor::firstUpdate()
{
    PROFILER_CPU();

	mCameraSceneObject = GET_SYSTEM(ScenesManager).getCameraSceneObject();
	mCameraSceneObject->mTransform->setLocalPosition(Maths::Vector3(0,0,100));
    EC::ComponentPtr<Camera> camera = ECManager.getFirstComponent<Camera>(mCameraSceneObject);
    Maths::Vector2 windowSize = GET_SYSTEM(Window::WindowManager).getMainWindow()->getWindowSize();
    // camera->setOrtho(-windowSize.x, windowSize.x, -windowSize.y, windowSize.y, -1000, 1000);

    // createPointLight(Maths::Vector3(0,50,0), 20);

    mDirectionalLight = createDirectionalLight(Maths::Vector3(0,2,0), Maths::Vector3::smForward + -Maths::Vector3::smUp);
    createSprite(Maths::Vector3(0,0,0), 100);
    // createSprite(Maths::Vector3(-100,0,0), 100);
    // createSprite(Maths::Vector3(100,0,0), 100);
    // createSprite(Maths::Vector3(0,0,-100), 10);

    // importModel("bob_lamp/bob_lamp_update.fbx", Maths::Vector3(0,0,-5), 1.0f);
	// sceneObject = importModel2("Avocado/glTF/Avocado.gltf", Maths::Vector3(150,0,0), 1000.0f, 0);
	// importModel("Floor/Floor.gltf", Maths::Vector3(0,0,0), 1.0f, Maths::Vector3(0,0,0), true);
	// importModel("Wall/Wall.gltf", Maths::Vector3(0,0,0), 1000.0f, Maths::Vector3(0,0,0), true);
	// importModel("Wall/Wall.gltf", Maths::Vector3(0,0,1000), 1.0f, Maths::Vector3(0,0,0), true);
	// importModel("BoxTextured/glTF//BoxTextured.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Maths::Vector3(150,0,0), 1000.0f, Maths::Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Maths::Vector3(0,-5,70), 1000.0f, Maths::Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Maths::Vector3(0,-5,-70), 1000.0f, Maths::Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avvocado.gltf", Maths::Vector3(-300,-5,0), 1000.0f, Maths::Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Maths::Vector3(150,-5,0), 1000.0f, Maths::Vector3(0,0,0), true);
	// importModel("Avocado/Instanced/Avocado.gltf", Maths::Vector3(-150,-5,0), 1000.0f, Maths::Vector3(0,0,0), true);
	// importModel("Bistro/Bistro.gltf", Maths::Vector3(0,0,0), 1.0f, 0, true);
	// importModel("Sponza/new/Sponza.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(0,0,0), true);
	// importModel("Building_1/building1.gltf", Maths::Vector3(50,0,0), 10.0f, Maths::Vector3(0,0,0), true);
	// importModel("Building_1/building1.gltf", Maths::Vector3(-50,0,50), 10.0f, Maths::Vector3(0,0,0), true);
	// importModel("building/building.gltf", Maths::Vector3(0,0,0), 10.0f, Maths::Vector3(0,0,0), true);
	// importModel("building/building.gltf", Maths::Vector3(-50,0,0), 10.0f, Maths::Vector3(0,0,0), true);
	// importModel("s5avfuixwjy8-calisma/Calisma2.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(0,0,0), true);
	// importModel("mountain/mount.blend1.gltf", Maths::Vector3(0,0,0), 500.0f, Maths::Vector3(0,0,0), true);
	// importModel("CesiumMan/glTF/CesiumMan.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(90,0,0), false);
	// importModel("CesiumMan/glTF/CesiumMan.gltf", Maths::Vector3(100,0,0), 100.0f, Maths::Vector3(90,0,0), false);
	// importModel("BrainStem/glTF/BrainStem.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(90,0,0), false);
    // mSceneObjectsArray.push_back(obj);
	
	importModel("CesiumMan/glTF/CesiumMan.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(90,0,0), false);
    Core::i32 size = 12;            
    FOR_RANGE(i, -size, size)
    {
        FOR_RANGE(j, -size, size)
        {
			// importModel("BrainStem/glTF/BrainStem.gltf", Maths::Vector3(150*i,0,150*j), 100.0f, Maths::Vector3(0,0,0), true);
            // if(j % 2 == 0)
            {
				// importModel("tower/tower.gltf", Maths::Vector3(500*i,3,500*j), 100.0f, Maths::Vector3(0,0,0), false);
				// importModel("CesiumMan/glTF/CesiumMan.gltf", Maths::Vector3(500*i,650,500*j), 100.0f, Maths::Vector3(90,0,0), false);
            }
            // else
            // {
			// 	importModel("cottage/cottage_blender.gltf", Maths::Vector3(250*i,0,250*j), 100.0f, Maths::Vector3(0,0,0), true);
            // }
        }
    }


	// obj = importModel("DamagedHelmet/glTF/DamagedHelmet.gltf", Maths::Vector3(0,270,0), 100.0f, Maths::Vector3(0,180,180), false);
    // mSceneObjectsArray.push_back(obj);
	// importModel("Fox/glTF/Fox.gltf", Maths::Vector3(300,0,0), 10.0f, Maths::Vector3(0,0,0), true);
	// importModel2("BrainStem/glTF/BrainStem.gltf", Maths::Vector3(0,0,0), 20.0f, 0);
	// importModel("bob_lamp/bob_lamp_update.gltf", Maths::Vector3(0,0,0), 20.0f, Maths::Vector3(0,0,0), true);
	// auto obj = importModel2("bob_lamp/bob_lamp_update.gltf", Maths::Vector3(0,-50,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Maths::Vector3(-300,0,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Maths::Vector3(0,0,0), 20.0f, 0);

    createUI();
    // mousePick();

    // mUISceneTree = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UISceneTree>();
    // mUISceneTree->mTransform->setLocalPosition(Maths::Vector2(-0.9, 0.8));
    // mUISceneTree->update();
}

void Editor::update()
{
	PROFILER_CPU()

    EC::ComponentPtr<Camera> camera = ECManager.getFirstComponent<Camera>(mCameraSceneObject);
	EC::ComponentPtr<Transform> cameraTransform = mCameraSceneObject->mTransform;
	Core::f32 speed = 400 * Time::Time::getInstance().getDeltaTimeSeconds();

	Maths::Matrix4 cameraRotationMatrix = mCameraSceneObject->mTransform->getLocalRotationMatrix();
	cameraRotationMatrix.invert();

	if(GET_SYSTEM(Input::Input).isKeyPressed(GLFW_KEY_LEFT))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Maths::Vector4(-speed,0,0,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Maths::Vector3(0,-speed,0));
        // cameraTransform->addLocalTranslation(Maths::Vector3(-speed,0,0));
	}
	else if (GET_SYSTEM(Input::Input).isKeyPressed(GLFW_KEY_RIGHT))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Maths::Vector4(speed,0,0,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Maths::Vector3(0,speed,0));
        // cameraTransform->addLocalTranslation(Maths::Vector3(speed,0,0));
	}
	else if (GET_SYSTEM(Input::Input).isKeyPressed(GLFW_KEY_UP))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Maths::Vector4(0,0,-speed,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Maths::Vector4(0,0,-speed,1));
        // cameraTransform->addLocalTranslation(Maths::Vector3(0,0,-speed));
		// cameraTransform->addLocalTranslation(Maths::Vector3(0,speed,0));
	}
	else if (GET_SYSTEM(Input::Input).isKeyPressed(GLFW_KEY_DOWN))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Maths::Vector4(0,0,speed,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Maths::Vector4(0,0,speed,1));
        // cameraTransform->addLocalTranslation(Maths::Vector3(0,0,speed));
		// cameraTransform->addLocalTranslation(Maths::Vector3(0,-speed,0));
	}
	else if (GET_SYSTEM(Input::Input).isKeyPressed(GLFW_KEY_PAGE_UP))
	{
		cameraTransform->addLocalTranslation(Maths::Vector3(0,speed,0));
	}
	else if (GET_SYSTEM(Input::Input).isKeyPressed(GLFW_KEY_PAGE_DOWN))
	{
		cameraTransform->addLocalTranslation(Maths::Vector3(0,-speed,0));
	}
    else if (GET_SYSTEM(Input::Input).isKeyPressed(GLFW_KEY_HOME))
	{
		// cameraTransform->addLocalRotation(Maths::Vector3(0,-speed,0));
        cameraTransform->lookAt(cameraTransform->getWorldPosition() + Maths::Vector3::smForward);
	}
	else if (GET_SYSTEM(Input::Input).isKeyPressed(GLFW_KEY_END))
	{
		// cameraTransform->addLocalRotation(Maths::Vector3(0,speed,0));
        cameraTransform->lookAt(cameraTransform->getWorldPosition() + -Maths::Vector3::smForward);
	}

    Maths::Vector2 currentMousePosition = GET_SYSTEM(Input::Input).getMousePosition();
    // currentMousePosition.set(-1,0,0);
    // LOG_VAR(currentMousePosition.x);
    // LOG_VAR(currentMousePosition.y);
    if(mSelectedSceneObject)
    {
        Maths::Vector3 position = camera->getGPUCamera().screenToWorld(currentMousePosition, mSelectedSceneObject->mTransform->getWorldPosition().z);
        // position.z = mSelectedSceneObject->mTransform->getLocalPosition().z;
        mSelectedSceneObject->mTransform->setLocalPosition(position);
        // LOG_VAR(position.x);
        // LOG_VAR(position.y);
        // LOG_VAR(position.z);
    }

	if(!mLastMousePosition.eq(currentMousePosition))
	{
        Core::f32 camSpeed = 200 * Time::Time::getInstance().getDeltaTimeSeconds();
		Maths::Vector2 mouseVector = (currentMousePosition - mLastMousePosition).nor() * camSpeed;
		Maths::Vector3 direction;

		Core::f32 yaw = mouseVector.x;
		Core::f32 pitch = mouseVector.y;

		cameraTransform->addLocalRotation(Maths::Vector3(pitch, -yaw, 0));
		// mDirectionalLight->mTransform->addLocalRotation(Maths::Vector3(0, -yaw, 0));
	}

	// LOG_VAR(cameraTransform->getLocalPosition().x)
	// LOG_VAR(cameraTransform->getLocalPosition().y)
	// LOG_VAR(cameraTransform->getLocalPosition().z)

	mLastMousePosition = currentMousePosition;

    //mDirectionalLight->mTransform->addLocalRotation(Maths::Vector3(0,0.1f,0));

    // PROFILER_CPU_NAMED("Draw Editor Lines");
    // // -x to x
	// GET_SYSTEM(DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(-1000,0,0), Maths::Vector3(1000,0,0)), 2, true, Maths::Vector4(1,0,0,1));

    // // x
	// GET_SYSTEM(DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(1000,0,0), Maths::Vector3(1000,100,0)), 1, true, Maths::Vector4(1,0,0,1));
    // // -x
	// GET_SYSTEM(DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(-1000,0,0), Maths::Vector3(-1000,50,0)), 1, true, Maths::Vector4(1,1,0,1));

    // // -y to y
	// GET_SYSTEM(DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(0,-1000,0), Maths::Vector3(0,1000,0)), 2, true, Maths::Vector4(0,1,0,1));

    // // -z to z
	// GET_SYSTEM(DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(0,0,-1000), Maths::Vector3(0,0,1000)), 2, true, Maths::Vector4(0,0,1,1));

    // // z
	// GET_SYSTEM(DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(0,0,1000), Maths::Vector3(0,100,1000)), 1, true, Maths::Vector4(0,0,1,1));
    // // -z
	// GET_SYSTEM(DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(0,0,-1000), Maths::Vector3(0,50,-1000)), 1, true, Maths::Vector4(0,1,1,1));

	for(Core::i32 x = -2000; x < 2000; x+=100)
	{
		GET_SYSTEM(DebugRenderer).drawLine(Maths::Line(Maths::Vector3(x,0,-2000), Maths::Vector3(x,0,2000)), 1, Maths::GeometricSpace::WORLD, Maths::Vector4(1,1,1,0.3f));
	}

    Core::f32 fps = 1000.0f/Time::Time::getInstance().getDeltaTimeMillis();
    // LOG_VAR(fps)
    if(mFPSCounter)
    {
        mFPSCounter->setText(Core::HashedString(std::to_string((Core::u32)fps)));
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

EC::EntityPtr<SceneObject> Editor::createSprite(const Maths::Vector3& v, Core::f32 size)
{
	EC::EntityPtr<SceneObject> sceneObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createSceneObject<SceneObject>();
	// sceneObject->mIsStatic = false;
	// sceneObject->mTransform->setLocalPosition(v);
	// sceneObject->mTransform->setLocalScale(Maths::Vector3(size,size,size));

    // RendererData rendererData;
	// rendererData.mMesh = GET_SYSTEM(GPUMeshFactory).getPrimitive<Maths::Cube>();

    GPUShaderData shaderData;
    shaderData.mGPUShaderTextureBindings.mTextureBindings.insert_or_assign(TextureBindingNames::smBaseColor, TextureBinding{"resources/snorlax-fill.png"});
	PropertiesBlockGPUShaderDefault shaderPropertiesBlock;
	// rendererData.mShader = (GET_SYSTEM(GPUShaderManager).createShader<GPUShaderDefault>(shaderData));

	// EC::ComponentPtr<MeshRenderer> renderer = ECManager.requestComponent<MeshRenderer>();
    // renderer->init(rendererData);
	// sceneObject->addComponent(renderer);

	GPURenderItemData rendererData;
    rendererData.mMesh = GPUInstance::getInstance().mGPUMeshFactory->getPrimitive<Maths::Cube>();
    rendererData.mShader = GPUInstance::getInstance().mGPUShaderManager->createShader<GPUShaderDefault, PropertiesBlockGPUShaderDefault>(GPUInstance::getInstance().mGPUContext, shaderData, shaderPropertiesBlock);
    rendererData.mRenderPassIDs = {
        Core::ClassManager::getClassMetadata<RenderPassGeometry>().mClassDefinition.getId(),
    };

	EC::ComponentPtr<MeshRenderer> renderer = ECManager.requestComponent<MeshRenderer>();
	renderer->init(rendererData);
    ECManager.addComponent(sceneObject, renderer);

	return sceneObject;
}

EC::EntityPtr<SceneObject> Editor::createPointLight(const Maths::Vector3& v, Core::f32 size)
{
	EC::EntityPtr<SceneObject> sceneObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createSceneObject<SceneObject>();
	sceneObject->mIsStatic = false;
	sceneObject->mTransform->setLocalPosition(v);
	sceneObject->mTransform->setLocalScale(Maths::Vector3(size,size,size));

    PointLightData data;
    data.mPosition = v;
    data.mDiffuse = Maths::Vector3(1,1,1) * 250000;

	EC::ComponentPtr<PointLight> pointLight = ECManager.requestComponent<PointLight>();
    pointLight->init(data);
    ECManager.addComponent(sceneObject, pointLight);

	return sceneObject;
}

EC::EntityPtr<SceneObject> Editor::createDirectionalLight(const Maths::Vector3& v, const Maths::Vector3& dir)
{
	EC::EntityPtr<SceneObject> sceneObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createSceneObject<SceneObject>();
    sceneObject->mIsStatic = false;
	sceneObject->mTransform->setLocalPosition(v);
	sceneObject->mTransform->lookAt(v + dir);

    DirectionalLightData directionalLightData;
    directionalLightData.mDirection = dir;
    directionalLightData.mDiffuse = Maths::Vector3(0.65,0.2,0.1) * 20;

	EC::ComponentPtr<DirectionalLight> dirLight = ECManager.requestComponent<DirectionalLight>();
    dirLight->init(directionalLightData);
    ECManager.addComponent(sceneObject, dirLight);

	return sceneObject;
}

EC::EntityPtr<SceneObject> Editor::mousePick()
{

    Core::f32 speed = 100 * Time::Time::getInstance().getDeltaTimeSeconds();
    EC::EntityPtr<SceneObject> obj;
    FOR_LIST(it, mSceneObjectsArray)
    {
        (*it)->mTransform->addLocalRotation(Maths::Vector3(0,0.1f,0));
        // const Maths::Cube& bbox = (*it)->getFirstComponent<MeshRenderer>()->getOcTreeBoundingBox();
        // EC::ComponentPtr<Camera> camera = mCameraSceneObject->getFirstComponent<Camera>();
        // Maths::Cube bboxScreenSpace(
        //     camera->worldToScreen(bbox.getLeftTopFront()),
        //     camera->worldToScreen(bbox.getLeftTopFront() + bbox.getSize()) - camera->worldToScreen(bbox.getLeftTopFront())
        // );

        // // GET_SYSTEM(DebugRenderer).drawCube(bboxScreenSpace, 1, false, Maths::Vector4(0.3,0,1,1));

        // Maths::Vector3 mousePosition = GET_SYSTEM(Input::Input).getMousePosition();
        // bool hit = Maths::Geometry::testCubePoint(bboxScreenSpace, mousePosition, 0);
        // if(hit)
        // {
        //     Maths::Cube hitMarker = Maths::Cube(mousePosition, Maths::Vector3(0.01,0.01,0.01));
        //     GET_SYSTEM(DebugRenderer).drawCube(hitMarker, 1, Maths::GeometricSpace::SCREEN, Maths::Vector4(1,0,0,1));
        //     GET_SYSTEM(DebugRenderer).drawCube(bboxScreenSpace, 1, Maths::GeometricSpace::SCREEN, Maths::Vector4(1,0,0,1));

        //     obj = *it;
        // }
    }

    return obj;
}

EC::EntityPtr<SceneObject> Editor::importModel( const std::string& pFile, const Maths::Vector3& v, Core::f32 size, const Maths::Vector3& rot, bool isStatic)
{
	Core::WeakPtr<const Model> model = GET_SYSTEM(ModelManager).loadModel(pFile);

    EC::EntityPtr<SceneObject> sceneObject = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultSceneName)->createSceneObject<SceneObject>();
	sceneObject->mIsStatic = isStatic;
	sceneObject->mTransform->setLocalPosition(v);
	sceneObject->mTransform->setLocalScale(Maths::Vector3::smOne * size);
	sceneObject->mTransform->setLocalRotation(rot);

    ModelRendererData modelRendererData;
    modelRendererData.mModel = model;
    modelRendererData.mRenderPassIDs =
    {
        Core::ClassManager::getClassMetadata<RenderPassGeometry>().mClassDefinition.getId(),
        Core::ClassManager::getClassMetadata<RenderPassShadowMap>().mClassDefinition.getId()
    };

	EC::ComponentPtr<ModelRenderer> modelRenderer = ECManager.requestComponent<ModelRenderer>();
    modelRenderer->init(modelRendererData);
    ECManager.addComponent(sceneObject, modelRenderer);

    return sceneObject;
}

void Editor::handlePressedKeys()
{

}

void Editor::handleMouse()
{
	if(GET_SYSTEM(Input::Input).isMouseButtonPressedOnce(GLFW_MOUSE_BUTTON_LEFT))
	{
        // GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeSceneObject(mBuildings.front());
        // mBuildings.pop_front();
        // EC::ComponentPtr<Camera> camera = mCameraSceneObject->getFirstComponent<Camera>();
        // Maths::Vector2 currentMousePosition = GET_SYSTEM(Input::Input).getMousePosition();
        // Maths::Vector3 position = camera->screenToWorld(currentMousePosition, 0);
        // auto obj = importModel("DamagedHelmet/glTF/DamagedHelmet.gltf", position, 1.0f, Maths::Vector3(0,180,180), false);
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
    
    if(GET_SYSTEM(Input::Input).isMouseButtonPressedOnce(GLFW_MOUSE_BUTTON_RIGHT))
    {
        // mBuildings.push_back(importModel("Building_1/building1.gltf", Maths::Vector3(0,100,0), 10.0f, Maths::Vector3(0,0,0), false));
        
    }
}

void Editor::createUI()
{
    mAxisViewer = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UIAxisGizmo>();
    mAxisViewer->mTransform->setLocalPosition(Maths::Vector2(-0.9, -0.8));
    mAxisViewer->createAxis();

    UIBuilder uiBuilder;

	uiBuilder.
	// setPosition(Maths::Vector2(0,0)).
	setPosition(Maths::Vector2(-1,1)).
	setAdjustSizeToText(true).
	setSize(Maths::Vector2(0.5, 0.05f));

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
	// setPosition(Maths::Vector2(0,0)).
	setPosition(Maths::Vector2(-1,1)).
	// setAdjustSizeToText(true).
	setSize(Maths::Vector2(0.5, 0.5f));

    // uiBuilder.
	// create<UIList>().
	// getUIElement<UIList>()->
	// addOption("A", [&](UIElement *uiElement)
	// {

	// }).
    // toggle();

    // mUITransform = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createSceneObject<UITransform>();
    // mUITransform->mTransform->setLocalPosition(Maths::Vector2(-0.7, -0.8));
    // mUITransform->mIsStatic = true;
    // mUIVector->update();
}
