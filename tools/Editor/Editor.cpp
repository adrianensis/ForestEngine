#include "Editor.hpp"
#include "Input/InputManager.hpp"
#include "Maths/Geometry.hpp"
#include "Maths/Vector3.hpp"
#include "GPU/Descriptors/GPUDescriptorLayout.hpp"
#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"
#include "Graphics/Mesh/MeshFactory.hpp"
#include "GPU/GPUInstance.hpp"
#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Graphics/Model/ModelRenderer.hpp"
#include "Scene/GameObject.hpp"
#include "UI/UIManager.hpp"
#include "Window/WindowManager.hpp"

#include "Scene/Module.hpp"
#include "UI/Module.hpp"
#include "Input/Input.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"

void Editor::init()
{
    PROFILER_CPU();
}

void Editor::firstUpdate(Core::f32 dt)
{
    PROFILER_CPU();

	GET_SYSTEM(Event::EventsManager).subscribe<Input::InputEventKeyPressed>(nullptr, this, [&](const Event::Event *event)
	{
		handlePressedKeys();
	});

    GET_SYSTEM(Event::EventsManager).subscribe<Input::InputEventMouseButtonHold>(nullptr, this, [&](const Event::Event *event)
	{
		handleMouse();
	});

    GET_SYSTEM(Event::EventsManager).subscribe<Input::InputEventMouseButtonReleased>(nullptr, this, [&](const Event::Event *event)
	{
		handleMouse();
	});

	mCameraGameObject = getOwnerGameObject()->mScene->getScenesManager()->getCameraGameObject();
	mCameraGameObject->mTransform->setLocalPosition(Maths::Vector3(0,0,100));
    Camera* camera = GET_SYSTEM(EC::EntityComponentManager).getFirstComponent<Camera>(mCameraGameObject);
    // Maths::Vector2 windowSize = (Window::WindowManager).getMainWindow()->getWindowSize();
    // camera->setOrtho(-windowSize.x, windowSize.x, -windowSize.y, windowSize.y, -1000, 1000);

	Transform* cameraTransform = mCameraGameObject->mTransform;
	mCurrentRotation = cameraTransform->getLocalRotation();
	mTargetRotation = mCurrentRotation;

    // createPointLight(Maths::Vector3(0,50,0), 20);

    mDirectionalLight = createDirectionalLight(Maths::Vector3(0,2,0), Maths::Vector3::smForward + -Maths::Vector3::smUp);
    createCube(Maths::Vector3(0,0,300), 100);
    createSprite(Maths::Vector3(0,0,0), 100);
    // createSprite(Maths::Vector3(-100,0,0), 100);
    // createSprite(Maths::Vector3(100,0,0), 100);
    // createSprite(Maths::Vector3(0,0,-100), 10);

    // importModel("bob_lamp/bob_lamp_update.fbx", Maths::Vector3(0,0,-5), 1.0f);
	// gameObject = importModel2("Avocado/glTF/Avocado.gltf", Maths::Vector3(150,0,0), 1000.0f, 0);
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
	importModel("Sponza/new/Sponza.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(0,0,0), true);
	// importModel("Building_1/building1.gltf", Maths::Vector3(50,0,0), 10.0f, Maths::Vector3(0,0,0), true);
	// importModel("Building_1/building1.gltf", Maths::Vector3(-50,0,50), 10.0f, Maths::Vector3(0,0,0), true);
	// importModel("building/building.gltf", Maths::Vector3(0,0,0), 10.0f, Maths::Vector3(0,0,0), true);
	// importModel("building/building.gltf", Maths::Vector3(-50,0,0), 10.0f, Maths::Vector3(0,0,0), true);
	// importModel("s5avfuixwjy8-calisma/Calisma2.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(0,0,0), true);
	// importModel("mountain/mount.blend1.gltf", Maths::Vector3(0,0,0), 500.0f, Maths::Vector3(0,0,0), true);
	// importModel("CesiumMan/glTF/CesiumMan.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(90,0,0), false);
	// importModel("CesiumMan/glTF/CesiumMan.gltf", Maths::Vector3(100,0,0), 100.0f, Maths::Vector3(90,0,0), false);
	// importModel("BrainStem/glTF/BrainStem.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(90,0,0), false);
    // mGameObjectsArray.push_back(obj);
	
	// importModel("CesiumMan/glTF/CesiumMan.gltf", Maths::Vector3(0,0,0), 100.0f, Maths::Vector3(90,0,0), false);
    Core::i32 size = 12;            
    FOR_RANGE(i, -size, size)
    {
        FOR_RANGE(j, -size, size)
        {
			// importModel("BrainStem/glTF/BrainStem.gltf", Maths::Vector3(150*i,0,150*j), 100.0f, Maths::Vector3(0,0,0), true);
            // if(j % 2 == 0)
            {
				// importModel("tower/tower.gltf", Maths::Vector3(500*i,3,500*j), 100.0f, Maths::Vector3(0,0,0), false);
				importModel("CesiumMan/glTF/CesiumMan.gltf", Maths::Vector3(200*i,100,200*j), 100.0f, Maths::Vector3(90,0,0), false);
            }
            // else
            // {
			// 	importModel("cottage/cottage_blender.gltf", Maths::Vector3(250*i,0,250*j), 100.0f, Maths::Vector3(0,0,0), true);
            // }
        }
    }


	// obj = importModel("DamagedHelmet/glTF/DamagedHelmet.gltf", Maths::Vector3(0,270,0), 100.0f, Maths::Vector3(0,180,180), false);
    // mGameObjectsArray.push_back(obj);
	// importModel("Fox/glTF/Fox.gltf", Maths::Vector3(300,0,0), 10.0f, Maths::Vector3(0,0,0), true);
	// importModel2("BrainStem/glTF/BrainStem.gltf", Maths::Vector3(0,0,0), 20.0f, 0);
	// importModel("bob_lamp/bob_lamp_update.gltf", Maths::Vector3(0,0,0), 20.0f, Maths::Vector3(0,0,0), true);
	// auto obj = importModel2("bob_lamp/bob_lamp_update.gltf", Maths::Vector3(0,-50,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Maths::Vector3(-300,0,0), 20.0f, 0);
	// importModel2("bob_lamp/bob_lamp_update.gltf", Maths::Vector3(0,0,0), 20.0f, 0);

    createUI();
    // mousePick();

    // mUISceneTree = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UISceneTree>();
    // mUISceneTree->mTransform->setLocalPosition(Maths::Vector2(-0.9, 0.8));
    // mUISceneTree->update();
}

void Editor::update(Core::f32 dt)
{
	PROFILER_CPU()

    Camera* camera = GET_SYSTEM(EC::EntityComponentManager).getFirstComponent<Camera>(mCameraGameObject);
	Transform* cameraTransform = mCameraGameObject->mTransform;
	Core::f32 speed = 400 * (dt/1000.0f);

	Maths::Matrix4 cameraRotationMatrix = mCameraGameObject->mTransform->getLocalRotationMatrix();
	cameraRotationMatrix.invert();

	if(GET_SYSTEM(Input::InputManager).getInput()->isKeyPressed(GLFW_KEY_LEFT))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Maths::Vector4(-speed,0,0,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Maths::Vector3(0,-speed,0));
        // cameraTransform->addLocalTranslation(Maths::Vector3(-speed,0,0));
	}
	else if (GET_SYSTEM(Input::InputManager).getInput()->isKeyPressed(GLFW_KEY_RIGHT))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Maths::Vector4(speed,0,0,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Maths::Vector3(0,speed,0));
        // cameraTransform->addLocalTranslation(Maths::Vector3(speed,0,0));
	}
	else if (GET_SYSTEM(Input::InputManager).getInput()->isKeyPressed(GLFW_KEY_UP))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Maths::Vector4(0,0,-speed,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Maths::Vector4(0,0,-speed,1));
        // cameraTransform->addLocalTranslation(Maths::Vector3(0,0,-speed));
		// cameraTransform->addLocalTranslation(Maths::Vector3(0,speed,0));
	}
	else if (GET_SYSTEM(Input::InputManager).getInput()->isKeyPressed(GLFW_KEY_DOWN))
	{
        cameraTransform->addLocalTranslation(cameraRotationMatrix.mulVector(Maths::Vector4(0,0,speed,1)));
        // mDirectionalLight->mTransform->addLocalRotation(Maths::Vector4(0,0,speed,1));
        // cameraTransform->addLocalTranslation(Maths::Vector3(0,0,speed));
		// cameraTransform->addLocalTranslation(Maths::Vector3(0,-speed,0));
	}
	else if (GET_SYSTEM(Input::InputManager).getInput()->isKeyPressed(GLFW_KEY_PAGE_UP))
	{
		cameraTransform->addLocalTranslation(Maths::Vector3(0,speed,0));
	}
	else if (GET_SYSTEM(Input::InputManager).getInput()->isKeyPressed(GLFW_KEY_PAGE_DOWN))
	{
		cameraTransform->addLocalTranslation(Maths::Vector3(0,-speed,0));
	}
    else if (GET_SYSTEM(Input::InputManager).getInput()->isKeyPressed(GLFW_KEY_HOME))
	{
		// cameraTransform->addLocalRotation(Maths::Vector3(0,-speed,0));
        cameraTransform->lookAt(cameraTransform->getWorldPosition() + Maths::Vector3::smForward);
	}
	else if (GET_SYSTEM(Input::InputManager).getInput()->isKeyPressed(GLFW_KEY_END))
	{
		// cameraTransform->addLocalRotation(Maths::Vector3(0,speed,0));
        cameraTransform->lookAt(cameraTransform->getWorldPosition() + -Maths::Vector3::smForward);
	}

    const Input::InputCursorPosition& currentCursorPosition = GET_SYSTEM(Input::InputManager).getInput()->getMousePosition();
    Maths::Vector2 currentMousePosition(currentCursorPosition.x, currentCursorPosition.y);
    // currentMousePosition.set(-1,0,0);
    // LOG_VAR(currentMousePosition.x);
    // LOG_VAR(currentMousePosition.y);
    if(mSelectedGameObject)
    {
        Maths::Vector3 position = camera->getGPUCamera().screenToWorld(currentMousePosition, mSelectedGameObject->mTransform->getWorldPosition().z);
        // position.z = mSelectedGameObject->mTransform->getLocalPosition().z;
        mSelectedGameObject->mTransform->setLocalPosition(position);
        // LOG_VAR(position.x);
        // LOG_VAR(position.y);
        // LOG_VAR(position.z);
    }

	Maths::Vector2 mouseDelta = (currentMousePosition - mLastMousePosition);
	mLastMousePosition = currentMousePosition;
	
	Core::f32 sensitivity = 100.0f;
	mTargetRotation.add(Maths::Vector3(mouseDelta.y * sensitivity, -mouseDelta.x * sensitivity, 0));
	
	Core::f32 lerpStep = 15.0f;
	mCurrentRotation.lerp(mTargetRotation, lerpStep * (dt / 1000.0f));

	cameraTransform->setLocalRotation(mTargetRotation);

	// LOG_VAR(cameraTransform->getLocalPosition().x)
	// LOG_VAR(cameraTransform->getLocalPosition().y)
	// LOG_VAR(cameraTransform->getLocalPosition().z)

    //mDirectionalLight->mTransform->addLocalRotation(Maths::Vector3(0,0.1f,0));

    // PROFILER_CPU_NAMED("Draw Editor Lines");
    // // -x to x
	// (DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(-1000,0,0), Maths::Vector3(1000,0,0)), 2, true, Maths::Vector4(1,0,0,1));

    // // x
	// (DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(1000,0,0), Maths::Vector3(1000,100,0)), 1, true, Maths::Vector4(1,0,0,1));
    // // -x
	// (DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(-1000,0,0), Maths::Vector3(-1000,50,0)), 1, true, Maths::Vector4(1,1,0,1));

    // // -y to y
	// (DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(0,-1000,0), Maths::Vector3(0,1000,0)), 2, true, Maths::Vector4(0,1,0,1));

    // // -z to z
	// (DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(0,0,-1000), Maths::Vector3(0,0,1000)), 2, true, Maths::Vector4(0,0,1,1));

    // // z
	// (DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(0,0,1000), Maths::Vector3(0,100,1000)), 1, true, Maths::Vector4(0,0,1,1));
    // // -z
	// (DebugRenderer).drawLine(Maths::Cube(Maths::Vector3(0,0,-1000), Maths::Vector3(0,50,-1000)), 1, true, Maths::Vector4(0,1,1,1));

	for(Core::i32 x = -2000; x < 2000; x+=100)
	{
		// (DebugRenderer).drawLine(Maths::Line(Maths::Vector3(x,0,-2000), Maths::Vector3(x,0,2000)), 1, Maths::GeometricSpace::WORLD, Maths::Vector4(1,1,1,0.3f));
	}

    Core::f32 fps = 1000.0f/dt;
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

GameObject* Editor::createCube(const Maths::Vector3& v, Core::f32 size)
{
	GameObject* gameObject = getOwnerGameObject()->mScene->createGameObject<GameObject>();
	// gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Maths::Vector3(size,size,size));

    // RendererData rendererData;
	// rendererData.mMesh = GET_SYSTEM(MeshFactory).getPrimitive<Maths::Cube>();

    GPUShaderData shaderData;
    GPUTextureHandle textureHandle = GPUInstance::getInstance().mGPUTextureManager->loadTexture(GPUTextureData{"resources/snorlax-fill.png"});
    shaderData.mTextureBindings.push_back(GPUTextureBinding{TextureBindingNames::smBaseColor, textureHandle});
	PropertiesBlockGPUShaderDefault shaderPropertiesBlock;
	// rendererData.mShader = (GET_SYSTEM(GPUShaderManager).createShader<GPUShaderDefault>(shaderData));

	// MeshRenderer* renderer = GET_SYSTEM(EC::EntityComponentManager).requestComponent<MeshRenderer>();
    // renderer->init(rendererData);
	// gameObject->addComponent(renderer);

	GPURenderItemData rendererData;
    rendererData.mMesh = MeshFactory::getInstance().getPrimitive<Maths::Cube>().getInternalPointer();
    rendererData.mShader = GPUInstance::getInstance().mGPUShaderManager->createShader<GPUShaderDefault, PropertiesBlockGPUShaderDefault>(GPUInstance::getInstance().mGPUContext, shaderData, shaderPropertiesBlock);
    rendererData.mRenderPassIDs = {
		GPURenderPass::getID<RenderPassGeometry>()
    };

	GET_SYSTEM(EC::EntityComponentManager).requestComponent<MeshRenderer>(gameObject, [&](auto* component)
	{
		component->init(rendererData);
	});

	return gameObject;
}

GameObject* Editor::createSprite(const Maths::Vector3& v, Core::f32 size)
{
	GameObject* gameObject = getOwnerGameObject()->mScene->createGameObject<GameObject>();
	// gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Maths::Vector3(size,size,size));

    // RendererData rendererData;
	// rendererData.mMesh = GET_SYSTEM(MeshFactory).getPrimitive<Maths::Cube>();

    GPUShaderData shaderData;
    GPUTextureHandle textureHandle = GPUInstance::getInstance().mGPUTextureManager->loadTexture(GPUTextureData{"resources/snorlax-fill.png"});
    shaderData.mTextureBindings.push_back(GPUTextureBinding{TextureBindingNames::smBaseColor, textureHandle});
	PropertiesBlockGPUShaderDefault shaderPropertiesBlock;
	// rendererData.mShader = (GET_SYSTEM(GPUShaderManager).createShader<GPUShaderDefault>(shaderData));

	// MeshRenderer* renderer = GET_SYSTEM(EC::EntityComponentManager).requestComponent<MeshRenderer>();
    // renderer->init(rendererData);
	// gameObject->addComponent(renderer);

	GPURenderItemData rendererData;
    rendererData.mMesh = MeshFactory::getInstance().getPrimitive<Maths::Rectangle>().getInternalPointer();
    rendererData.mShader = GPUInstance::getInstance().mGPUShaderManager->createShader<GPUShaderDefault, PropertiesBlockGPUShaderDefault>(GPUInstance::getInstance().mGPUContext, shaderData, shaderPropertiesBlock);
    rendererData.mRenderPassIDs = {
		GPURenderPass::getID<RenderPassGeometry>()
    };

	GET_SYSTEM(EC::EntityComponentManager).requestComponent<MeshRenderer>(gameObject, [&](auto* component)
	{
		component->init(rendererData);
	});

	return gameObject;
}

GameObject* Editor::createPointLight(const Maths::Vector3& v, Core::f32 size)
{
	GameObject* gameObject = getOwnerGameObject()->mScene->createGameObject<GameObject>();
	gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Maths::Vector3(size,size,size));

    PointLightData data;
    data.mPosition = v;
    data.mDiffuse = Maths::Vector3(1,1,1) * 250000;

	GET_SYSTEM(EC::EntityComponentManager).requestComponent<PointLight>(gameObject, [&](auto* component)
	{
		component->init(data);
	});

	return gameObject;
}

GameObject* Editor::createDirectionalLight(const Maths::Vector3& v, const Maths::Vector3& dir)
{
	GameObject* gameObject = getOwnerGameObject()->mScene->createGameObject<GameObject>();
    gameObject->mIsStatic = false;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->lookAt(v + dir);

    DirectionalLightData directionalLightData;
    directionalLightData.mDirection = dir;
    directionalLightData.mDiffuse = Maths::Vector3(0.65,0.2,0.1) * 20;

	GET_SYSTEM(EC::EntityComponentManager).requestComponent<DirectionalLight>(gameObject, [&](auto* component)
	{
		component->init(directionalLightData);
	});

	return gameObject;
}

GameObject* Editor::mousePick()
{

    // Core::f32 speed = 100 * dt;
    GameObject* obj;
    FOR_LIST(it, mGameObjectsArray)
    {
        (*it)->mTransform->addLocalRotation(Maths::Vector3(0,0.1f,0));
        // const Maths::Cube& bbox = (*it)->getFirstComponent<MeshRenderer>()->getOcTreeBoundingBox();
        // Camera* camera = mCameraGameObject->getFirstComponent<Camera>();
        // Maths::Cube bboxScreenSpace(
        //     camera->worldToScreen(bbox.getLeftTopFront()),
        //     camera->worldToScreen(bbox.getLeftTopFront() + bbox.getSize()) - camera->worldToScreen(bbox.getLeftTopFront())
        // );

        // // (DebugRenderer).drawCube(bboxScreenSpace, 1, false, Maths::Vector4(0.3,0,1,1));

        // Maths::Vector3 mousePosition = GET_SYSTEM(Input::InputManager).getInput()->getMousePosition();
        // bool hit = Maths::Geometry::testCubePoint(bboxScreenSpace, mousePosition, 0);
        // if(hit)
        // {
        //     Maths::Cube hitMarker = Maths::Cube(mousePosition, Maths::Vector3(0.01,0.01,0.01));
        //     (DebugRenderer).drawCube(hitMarker, 1, Maths::GeometricSpace::SCREEN, Maths::Vector4(1,0,0,1));
        //     (DebugRenderer).drawCube(bboxScreenSpace, 1, Maths::GeometricSpace::SCREEN, Maths::Vector4(1,0,0,1));

        //     obj = *it;
        // }
    }

    return obj;
}

GameObject* Editor::importModel( const std::string& pFile, const Maths::Vector3& v, Core::f32 size, const Maths::Vector3& rot, bool isStatic)
{
	Core::WeakPtr<const Model> model = GET_SYSTEM(ModelManager).loadModel(pFile);

    GameObject* gameObject = getOwnerGameObject()->mScene->createGameObject<GameObject>();
	gameObject->mIsStatic = isStatic;
	gameObject->mTransform->setLocalPosition(v);
	gameObject->mTransform->setLocalScale(Maths::Vector3::smOne * size);
	gameObject->mTransform->setLocalRotation(rot);

    ModelRendererData modelRendererData;
    modelRendererData.mModel = model;
    modelRendererData.mRenderPassIDs =
    {
		GPURenderPass::getID<RenderPassGeometry>(),
		GPURenderPass::getID<RenderPassShadowMap>()
    };

	GET_SYSTEM(EC::EntityComponentManager).requestComponent<ModelRenderer>(gameObject, [&](auto* component)
	{
		component->init(modelRendererData);
	});

    return gameObject;
}

void Editor::handlePressedKeys()
{

}

void Editor::handleMouse()
{
	if(GET_SYSTEM(Input::InputManager).getInput()->isMouseButtonPressedOnce(GLFW_MOUSE_BUTTON_LEFT))
	{
        // GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->removeGameObject(mBuildings.front());
        // mBuildings.pop_front();
        // Camera* camera = mCameraGameObject->getFirstComponent<Camera>();
        // Maths::Vector2 currentMousePosition = GET_SYSTEM(Input::InputManager).getInput()->getMousePosition();
        // Maths::Vector3 position = camera->screenToWorld(currentMousePosition, 0);
        // auto obj = importModel("DamagedHelmet/glTF/DamagedHelmet.gltf", position, 1.0f, Maths::Vector3(0,180,180), false);
        // mGameObjectsArray.push_back(obj);
        if(!mSelectedGameObject)
        {
            mSelectedGameObject = mousePick();
        }
	}
    else
    {
        mSelectedGameObject = nullptr;
    }
    
    if(GET_SYSTEM(Input::InputManager).getInput()->isMouseButtonPressedOnce(GLFW_MOUSE_BUTTON_RIGHT))
    {
        // mBuildings.push_back(importModel("Building_1/building1.gltf", Maths::Vector3(0,100,0), 10.0f, Maths::Vector3(0,0,0), false));
        
    }
}

void Editor::createUI()
{
    mAxisViewer = getOwnerGameObject()->mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UIAxisGizmo>();
    mAxisViewer->mTransform->setLocalPosition(Maths::Vector2(-0.9, -0.8));
    mAxisViewer->createAxis();

    UIBuilder uiBuilder = GET_SYSTEM(UIManager).createUIBuilder();

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
	create<UIText>(getOwnerGameObject()->mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer()).
    getUIElement<UIText>();

	uiBuilder.
	setText("File").
	create<UIDropdown>(getOwnerGameObject()->mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer()).
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
	create<UIButton>(getOwnerGameObject()->mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer()).
	getUIElement<UIButton>()->
	setOnPressedCallback([&, this](UIElement *uiElement){
	});

	uiBuilder.
	setText("Edit").
	create<UIButton>(getOwnerGameObject()->mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer()).
	getUIElement<UIButton>()->
	setOnPressedCallback([&, this](UIElement *uiElement){
	});

	uiBuilder.
	setText("View").
	create<UIDropdown>(getOwnerGameObject()->mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer()).
	getUIElement<UIDropdown>()->
	addOption("Grid", [&](UIElement *uiElement)
	{

	}).
	addOption("Colliders", [&](UIElement *uiElement)
	{
	});

    uiBuilder.
	setText("a").
	create<UIEditableText>(getOwnerGameObject()->mScene->getScenesManager()->getScene(ScenesManager::smDefaultUISceneName).getInternalPointer());

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

    // mUITransform = GET_SYSTEM(ScenesManager).getScene(ScenesManager::smDefaultUISceneName)->createGameObject<UITransform>();
    // mUITransform->mTransform->setLocalPosition(Maths::Vector2(-0.7, -0.8));
    // mUITransform->mIsStatic = true;
    // mUIVector->update();
}
