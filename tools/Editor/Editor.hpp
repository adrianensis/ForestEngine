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
	Ptr<GameObject> createSprite(const Vector3& v, f32 size);
	Ptr<GameObject> createPointLight(const Vector3& v, f32 size);
	Ptr<GameObject> createDirectionalLight(const Vector3& v, const Vector3& dir);
	Ptr<GameObject> mousePick();
    Ptr<GameObject> importModel(const std::string& pFile, const Vector3& v, f32 size, const Vector3& rot, bool isStatic);
	void handlePressedKeys();
	void handleMouse();
    void createUI();
private:
    Ptr<UIText> mFPSCounter;
    Ptr<UITransform> mUITransform;
    Ptr<UISceneTree> mUISceneTree;
	Ptr<GameObject> mCameraGameObject;
	Ptr<GameObject> mDirectionalLight;
	Ptr<GameObject> mSelectedGameObject;
	Ptr<UIAxisGizmo> mAxisViewer;

    std::vector<Ptr<GameObject>> mGameObjectsArray;

    Vector2 mLastMousePosition;
};
REGISTER_CLASS(Editor)