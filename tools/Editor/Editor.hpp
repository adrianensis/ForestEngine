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
	EC::EntityPtr<SceneObject> createSprite(const Maths::Vector3& v, Core::f32 size);
	EC::EntityPtr<SceneObject> createPointLight(const Maths::Vector3& v, Core::f32 size);
	EC::EntityPtr<SceneObject> createDirectionalLight(const Maths::Vector3& v, const Maths::Vector3& dir);
	EC::EntityPtr<SceneObject> mousePick();
    EC::EntityPtr<SceneObject> importModel(const std::string& pFile, const Maths::Vector3& v, Core::f32 size, const Maths::Vector3& rot, bool isStatic);
	void handlePressedKeys();
	void handleMouse();
    void createUI();
private:
    EC::EntityPtr<UIText> mFPSCounter;
    EC::EntityPtr<UITransform> mUITransform;
    EC::EntityPtr<UISceneTree> mUISceneTree;
	EC::EntityPtr<SceneObject> mCameraSceneObject;
	EC::EntityPtr<SceneObject> mDirectionalLight;
	EC::EntityPtr<SceneObject> mSelectedSceneObject;
	std::list<EC::EntityPtr<SceneObject>> mBuildings;
	EC::EntityPtr<UIAxisGizmo> mAxisViewer;

    std::vector<EC::EntityPtr<SceneObject>> mSceneObjectsArray;

    Maths::Vector2 mLastMousePosition;
};
REGISTER_CLASS(Editor)