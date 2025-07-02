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
	EC::EntityPtr createSprite(const Maths::Vector3& v, Core::f32 size);
	EC::EntityPtr createPointLight(const Maths::Vector3& v, Core::f32 size);
	EC::EntityPtr createDirectionalLight(const Maths::Vector3& v, const Maths::Vector3& dir);
	EC::EntityPtr mousePick();
    EC::EntityPtr importModel(const std::string& pFile, const Maths::Vector3& v, Core::f32 size, const Maths::Vector3& rot, bool isStatic);
	void handlePressedKeys();
	void handleMouse();
    void createUI();
private:
    EC::TEntityPtr<UIText> mFPSCounter;
    EC::TEntityPtr<UITransform> mUITransform;
    EC::TEntityPtr<UISceneTree> mUISceneTree;
	EC::TEntityPtr<SceneObject> mCameraSceneObject;
	EC::TEntityPtr<SceneObject> mDirectionalLight;
	EC::TEntityPtr<SceneObject> mSelectedSceneObject;
	std::list<EC::TEntityPtr<SceneObject>> mBuildings;
	EC::TEntityPtr<UIAxisGizmo> mAxisViewer;

    std::vector<EC::TEntityPtr<SceneObject>> mSceneObjectsArray;

    Maths::Vector2 mLastMousePosition;
};
REGISTER_CLASS(Editor)