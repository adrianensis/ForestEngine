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
	EC::EntityPtr<GameObject> createSprite(const Maths::Vector3& v, Core::f32 size);
	EC::EntityPtr<GameObject> createPointLight(const Maths::Vector3& v, Core::f32 size);
	EC::EntityPtr<GameObject> createDirectionalLight(const Maths::Vector3& v, const Maths::Vector3& dir);
	EC::EntityPtr<GameObject> mousePick();
    EC::EntityPtr<GameObject> importModel(const std::string& pFile, const Maths::Vector3& v, Core::f32 size, const Maths::Vector3& rot, bool isStatic);
	void handlePressedKeys();
	void handleMouse();
    void createUI();
private:
    EC::EntityPtr<UIText> mFPSCounter;
    EC::EntityPtr<UITransform> mUITransform;
    EC::EntityPtr<UISceneTree> mUISceneTree;
	EC::EntityPtr<GameObject> mCameraGameObject;
	EC::EntityPtr<GameObject> mDirectionalLight;
	EC::EntityPtr<GameObject> mSelectedGameObject;
	std::list<EC::EntityPtr<GameObject>> mBuildings;
	EC::EntityPtr<UIAxisGizmo> mAxisViewer;

    std::vector<EC::EntityPtr<GameObject>> mGameObjectsArray;

    Maths::Vector2 mLastMousePosition;
};
REGISTER_CLASS(Editor, Script)