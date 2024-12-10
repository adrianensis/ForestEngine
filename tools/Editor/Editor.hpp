#pragma once

#include "Scripting/Script.hpp"
#include "UI/Module.hpp"
#include "UIAxisGizmo.hpp"
#include "UITransform.hpp"
#include "UISceneTree.hpp"

class Editor: public Script
{
public:
    void init();
    void firstUpdate();
    void update();
    void terminate();

private:
	EntityHandler createSprite(const Vector3& v, f32 size);
	EntityHandler createPointLight(const Vector3& v, f32 size);
	EntityHandler createDirectionalLight(const Vector3& v, const Vector3& dir);
	EntityHandler mousePick();
    EntityHandler importModel(const std::string& pFile, const Vector3& v, f32 size, const Vector3& rot, bool isStatic);
	void handlePressedKeys();
	void handleMouse();
    void createUI();
private:
    TEntityHandler<UIText> mFPSCounter;
    TEntityHandler<UITransform> mUITransform;
    TEntityHandler<UISceneTree> mUISceneTree;
	TEntityHandler<GameObject> mCameraGameObject;
	TEntityHandler<GameObject> mDirectionalLight;
	TEntityHandler<GameObject> mSelectedGameObject;
	std::list<TEntityHandler<GameObject>> mBuildings;
	TEntityHandler<UIAxisGizmo> mAxisViewer;

    std::vector<TEntityHandler<GameObject>> mGameObjectsArray;

    Vector2 mLastMousePosition;
};
REGISTER_CLASS(Editor)