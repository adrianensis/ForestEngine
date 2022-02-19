#pragma once

#include "Core/Module.hpp"
#include "EditorUI/Brush.hpp"
#include "EditorUI/Grid.hpp"
#include "EditorUI/MenuBar.hpp"
#include "EditorUI/InfoBar.hpp"
#include "EditorUI/ToolsBar.hpp"
#include "EditorUI/LayersBar.hpp"
#include "EditorUI/Sprites.hpp"
#include "EditorUI/EditorStyles.hpp"

#include "SpacePartition/KTree.hpp"

class Material;
class GameObject;
class Camera;

using TileCallback = std::function<void(GameObject* tile)>;

class EditorController: public ObjectBase
{
    GENERATE_METADATA(EditorController)
	PRI ConfigObject mEditorConfig; GET_RC(EditorConfig)

	PRI Brush mBrush; GET_R(Brush)
	PRI MenuBar mMenuBar; GET_R(MenuBar)
	PRI InfoBar mInfoBar; GET_R(InfoBar)
	PRI ToolsBar mToolsBar; GET_R(ToolsBar)
	PRI LayersBar mLayersBar; GET_R(LayersBar)
	PRI Sprites mSprites; GET_R(Sprites)

	PRI std::list<GameObject*> mSelectedTiles; GET_R(SelectedTiles)

	PRI u32 mLayer = 0; GET_SET(Layer)
	PRI std::vector<Grid> mGrids;
	PRI bool mDrawGrid = false; GET_SET(DrawGrid)

	PRI Camera* mCamera = nullptr; GET(Camera)
	PRI f32 mCameraSpeed = 0.0f;
	PRI bool mCameraDragStarted = false;
	PRI Vector3 mCameraDragLastPosition;

	QuadTree quadTree;

PUB

	void init();
	void update();
	void drawGrid();
	bool canUseBrush() const;

	Grid& getGrid();

	GameObject* createTile(const Vector2& position, const Vector2& size, Material *material, const Rectangle& region);

	void forEachSelectedTile(TileCallback tileCallback);

	void saveScene();
	void loadScene();

	void moveCameraKeys();
	void moveCameraMouse();
	void releaseCameraMouse();
	void zoom(f32 scroll);
	void handleHoldKeys();
	void handlePressedKeys();
	void toggle();
};