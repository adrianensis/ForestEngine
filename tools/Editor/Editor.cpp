#include "Editor.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Module.hpp"
#include "UI/Module.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h" // Output data structure
#include "assimp/postprocess.h" // Post processing flags


void Editor::init()
{
	octree.init(1000);
}

void Editor::firstUpdate()
{

	i32 size = 4;
	for(i32 x = 0; x < size; ++x)
	{
	}

	importModel("resources/wolf/WOLF.OBJ");


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

void Editor::update()
{
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
}

void Editor::terminate()
{
	
}

GameObject* Editor::createSprite(const Vector3& v, f32 size)
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

void Editor::importModel( const std::string& pFile)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( pFile, 
			//aiProcess_CalcTangentSpace       | 
			aiProcess_Triangulate            |
			aiProcess_JoinIdenticalVertices  |
			aiProcess_SortByPType);
	
	if(scene)
	{
		aiVector3t<f32> assimpPosition;
		aiVector3t<f32> assimpScale;
		aiVector3t<f32> assimpRotation;

		scene->mRootNode->mTransformation.Decompose(assimpScale, assimpRotation, assimpPosition);

		Vector3 position = Vector3(assimpPosition.x, assimpPosition.y, assimpPosition.z);
		Vector3 scale = Vector3(assimpScale.x, assimpScale.y, assimpScale.z);
		Vector3 rotation = Vector3(assimpRotation.x, assimpRotation.y, assimpRotation.z);

		GameObject* gameObject = NEW(GameObject);
		gameObject->init();
		gameObject->setIsStatic(false);
		gameObject->getTransform().get().setLocalPosition(Vector3(0,0,0));
		gameObject->getTransform().get().setScale(scale*20);

		mesh = OwnerPtr<Mesh>(NEW(Mesh));

		if(scene->HasMeshes())
		{


			aiMesh* assimpMesh = scene->mMeshes[0];

			mesh.get().init(assimpMesh->mNumVertices, assimpMesh->mNumFaces);

			FOR_RANGE(vertexIt, 0, assimpMesh->mNumVertices)
			{
				aiVector3D assimpVertex = assimpMesh->mVertices[vertexIt];
				Vector3 vertex = Vector3(assimpVertex.x, assimpVertex.y, assimpVertex.z);
				mesh.get().addVertex(vertex);

				aiVector3D assimpTexCoord = assimpMesh->mTextureCoords[0][vertexIt];

				Vector2 texCoord = Vector2(assimpTexCoord.x, assimpTexCoord.y);
				mesh.get().addTexCoord(texCoord.x, texCoord.y);
			}

			FOR_RANGE(faceIt, 0, assimpMesh->mNumFaces)
			{
				aiFace assimpFace = assimpMesh->mFaces[faceIt];
				mesh.get().addFace(assimpFace.mIndices[0], assimpFace.mIndices[1], assimpFace.mIndices[2]);
			}
		}

		Renderer *renderer = NEW(Renderer);
		renderer->init();

		renderer->setMesh(Ptr<Mesh>(mesh));
		renderer->setMaterial(MaterialManager::getInstance().loadMaterial("resources/wolf/WOLF.png"));
		//renderer->setMaterial(MaterialManager::getInstance().loadNoTextureMaterial());
		//renderer->setColor(Vector4(1,1,0,1));

		gameObject->addComponent<Renderer>(renderer);

		ScenesManager::getInstance().getCurrentScene()->addGameObject(gameObject);
	}
}
