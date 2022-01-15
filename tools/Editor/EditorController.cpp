#include "EditorController.hpp"
#include "EditorCommands.hpp"

#include "Scenes/Scenes.hpp"
#include "Graphics/Graphics.hpp"


void EditorController::init()
{
    EditorCommands::registerEditorCommands(this);
    
	mEditorConfig.readFromJsonFile("tools/Editor/config/resources.json");

	mGrids.resize(10); // TODO : engine max layers
	
	FOR_ARRAY(i, mGrids)
    {
        mGrids[i].init(this);
    }

	mBrush.init(this);
	mBrush.setBrushSize(20);
	mInfoBar.init(this);
	mToolsBar.init(this);
	mLayersBar.init(this);
	mMenuBar.init(this);
	mSprites.init(this);

	SUBSCRIBE_TO_EVENT(InputEventKeyTab, nullptr, this, [&](const Event *event)
	{
		toggle();
	});

	SUBSCRIBE_TO_EVENT(InputEventKeyPressed, nullptr, this, [&](const Event *event)
	{
		handlePressedKeys();
	});

	SUBSCRIBE_TO_EVENT(InputEventKeyHold, nullptr, this, [&](const Event *event)
	{
		handleHoldKeys();
	});

	SUBSCRIBE_TO_EVENT(InputEventScroll, nullptr, this, [&](const Event *event)
	{
		zoom(Input::getInstance().getScroll());
	});

	SUBSCRIBE_TO_EVENT(InputEventMouseButtonHold, nullptr, this, [&](const Event *event)
	{
		InputEventMouseButtonHold* e = (InputEventMouseButtonHold*) event;
		if(e->mButton == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			moveCameraMouse();
		}
	});

	SUBSCRIBE_TO_EVENT(InputEventMouseButtonReleased, nullptr, this, [&](const Event *event)
	{
		InputEventMouseButtonReleased* e = (InputEventMouseButtonReleased*) event;
		if(e->mButton == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			releaseCameraMouse();
		}
	});

	mDrawGrid = true;

	mCamera = ScenesManager::getInstance().getCurrentScene()->
	getCameraGameObject()->getFirstComponent<Camera>();

	mCameraSpeed = 1000;

	//quadTree.init(50*100);
}

void EditorController::update()
{
	//quadTree.update();
	drawGrid();
	mInfoBar.setFPS(1.0f/Time::getInstance().getDeltaTimeSeconds());
}

Grid& EditorController::getGrid()
{
	return mGrids[mLayer];
}

void EditorController::drawGrid()
{
	if(mDrawGrid)
	{
		Vector4 gridColor = Vector4(1,1,1,0.25f);

		Vector2 gridSize = getGrid().getGridSize();
		Vector2 halfGridSize = gridSize / 2.0f;
		Vector2 tileSize = getGrid().getTileSize();
		Vector2 halfTileSize = tileSize / 2.0f;

		FOR_RANGE(i, -halfGridSize.x, halfGridSize.x + 1)
		{
			RenderEngine::getInstance().drawLine(
				Line(Vector3(i * tileSize.y - halfTileSize.x, -halfGridSize.y * tileSize.y - halfTileSize.y,0), 
				Vector3(i * tileSize.y - halfTileSize.x, halfGridSize.y * tileSize.y - halfTileSize.y,0)),
				1,
				true,
				gridColor);
		}

		FOR_RANGE(j, -halfGridSize.y - 1, halfGridSize.y)
		{
			RenderEngine::getInstance().drawLine(
				Line(Vector3(-halfGridSize.x * tileSize.x - halfTileSize.x, j * tileSize.y + halfTileSize.y ,0), 
				Vector3(halfGridSize.x * tileSize.x - halfTileSize.x, j * tileSize.y + halfTileSize.y ,0)),
				1,
				true,
				gridColor);
		}
	}
}

GameObject* EditorController::createTile(const Vector2& position, const Vector2& size, Material *material, const Rectangle2D& region)
{
	GameObject *tile = NEW(GameObject);
	tile->init();

	tile->setShouldPersist(true);
	tile->setIsStatic(true);

	tile->getTransform()->setLocalPosition(position);
	tile->getTransform()->setScale(Vector3(size.x, size.y, 1));

	Renderer *renderer = NEW(Renderer);
	renderer->init();

	renderer->setMesh(MeshPrimitives::getInstance().getOrCreatePrimitive<Rectangle2D>());
	renderer->setDepth(-1 * getLayer());

	renderer->setMaterial(material);
	renderer->setRegion(region);

	tile->addComponent<Renderer>(renderer);
	
	ScenesManager::getInstance().getCurrentScene()->addGameObject(tile);

	return tile;
}

bool EditorController::canUseBrush() const
{
	return !mSprites.getIsVisible();
}

void EditorController::forEachSelectedTile(TileCallback tileCallback)
{
	FOR_LIST(it, mSelectedTiles)
	{
		tileCallback(*it);
	}
}

void EditorController::saveScene()
{
    mSprites.saveSprites();
	ScenesManager::getInstance().getCurrentScene()->saveScene("config/tmp.json");
}

void EditorController::loadScene()
{
	Scene *scene = ScenesManager::getInstance().getCurrentScene();
	scene->loadScene("config/tmp.json");

	std::list<GameObject *> tmpList = scene->getNewGameObjects();
	FOR_LIST(it, tmpList)
	{
		GameObject* gameObject = (*it);

		Renderer *renderer = gameObject->getFirstComponent<Renderer>();

		mGrids[renderer->getDepth()].setCell(mGrids[renderer->getDepth()].calculateGridPosition(gameObject->getTransform()->getWorldPosition()), 
			gameObject
		);
	}
}

void EditorController::moveCameraKeys()
{
	Transform* cameraTransform = mCamera->getGameObject()->getTransform();

	f32 speed = mCameraSpeed * Time::getInstance().getDeltaTimeSeconds();

	if(Input::getInstance().isKeyPressed(GLFW_KEY_LEFT))
	{
		cameraTransform->translate(Vector2(-speed,0));
	}
	else if (Input::getInstance().isKeyPressed(GLFW_KEY_RIGHT))
	{
		cameraTransform->translate(Vector2(speed,0));
	}
	else if (Input::getInstance().isKeyPressed(GLFW_KEY_UP))
	{
		cameraTransform->translate(Vector2(0,speed));
	}
	else if (Input::getInstance().isKeyPressed(GLFW_KEY_DOWN))
	{
		cameraTransform->translate(Vector2(0,-speed));
	}
}

void EditorController::moveCameraMouse()
{
	Vector2 mouse = Input::getInstance().getMousePosition();
	Vector2 worldPosition = mCamera->screenToWorld(mouse);

	if(!mCameraDragStarted)
	{
		mCameraDragStarted = true;
		mCameraDragLastPosition = worldPosition;
	}

	Transform* cameraTransform = mCamera->getGameObject()->getTransform();

	Vector2 dirVector = worldPosition - mCameraDragLastPosition;

	if(dirVector.len() > 0)
	{
		dirVector.nor();

		f32 speed = mCameraSpeed * Time::getInstance().getDeltaTimeSeconds();

		cameraTransform->translate(dirVector * speed);
	}

	mCameraDragLastPosition = worldPosition;
}

void EditorController::releaseCameraMouse()
{
	mCameraDragStarted = false;
	mCameraDragLastPosition.set(0,0,0);
}

void EditorController::zoom(f32 scroll)
{
	f32 zoomDelta = 10.0f * Time::getInstance().getDeltaTimeSeconds();

	if(scroll > 0)
	{
		mCamera->zoomIn(zoomDelta);
	}
	else
	{
		mCamera->zoomOut(zoomDelta);
	}
}

void EditorController::handlePressedKeys()
{
	if(Input::getInstance().isKeyPressedOnce(GLFW_KEY_KP_ADD))
	{
		if(Input::getInstance().isModifierPressed(GLFW_MOD_CONTROL))
		{
			mBrush.setBrushSize(mBrush.getBrushSize() + 1);
			mInfoBar.setBrushSize(mBrush.getBrushSize());
		}
	}

	if(Input::getInstance().isKeyPressedOnce(GLFW_KEY_KP_SUBTRACT))
	{
		if(Input::getInstance().isModifierPressed(GLFW_MOD_CONTROL))
		{
			mBrush.setBrushSize(mBrush.getBrushSize() - 1);
			mInfoBar.setBrushSize(mBrush.getBrushSize());
		}
	}

    if(Input::getInstance().isKeyPressedOnce(GLFW_KEY_F1))
	{
        CommandLine::getInstance().toggle();
    }
}

void EditorController::handleHoldKeys()
{
	moveCameraKeys();

	if(!Input::getInstance().isKeyPressedOnce(GLFW_KEY_KP_ADD) &&
		!Input::getInstance().isModifierPressed(GLFW_MOD_CONTROL))
	{
		if(Input::getInstance().isKeyPressed(GLFW_KEY_KP_ADD))
		{
			zoom(1);
		}
	}

	if(!Input::getInstance().isKeyPressedOnce(GLFW_KEY_KP_SUBTRACT) &&
		!Input::getInstance().isModifierPressed(GLFW_MOD_CONTROL))
	{
		if(Input::getInstance().isKeyPressed(GLFW_KEY_KP_SUBTRACT))
		{
			zoom(-1);
		}
	}
}

void EditorController::toggle()
{
	mSprites.toggle();
	mToolsBar.toggle();
	mLayersBar.toggle();
}