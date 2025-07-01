#pragma once

#include "Scripting/Script.hpp"
#include "UI/Module.hpp"
#include "UIAxisGizmo.hpp"
#include "UITransform.hpp"
#include "UISceneTree.hpp"

class Editor: public Script
{
public:
    virtual void init() override;
    virtual void firstUpdate() override;
    virtual void update() override;
    virtual void terminate() override;

private:
	EntityPtr createSprite(const Vector3& v, Core::f32 size);
	EntityPtr createPointLight(const Vector3& v, Core::f32 size);
	EntityPtr createDirectionalLight(const Vector3& v, const Vector3& dir);
	EntityPtr mousePick();
    EntityPtr importModel(const std::string& pFile, const Vector3& v, Core::f32 size, const Vector3& rot, bool isStatic);
	void handlePressedKeys();
	void handleMouse();
    void createUI();
private:
    TEntityPtr<UIText> mFPSCounter;
    TEntityPtr<UITransform> mUITransform;
    TEntityPtr<UISceneTree> mUISceneTree;
	TEntityPtr<SceneObject> mCameraSceneObject;
	TEntityPtr<SceneObject> mDirectionalLight;
	TEntityPtr<SceneObject> mSelectedSceneObject;
	std::list<TEntityPtr<SceneObject>> mBuildings;
	TEntityPtr<UIAxisGizmo> mAxisViewer;

    std::vector<TEntityPtr<SceneObject>> mSceneObjectsArray;

    Vector2 mLastMousePosition;
};
REGISTER_CLASS(Editor)