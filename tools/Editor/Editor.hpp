#pragma once

#include "Core/TypeMacros.hpp"
#include "Scripting/Script.hpp"
#include "UI/Module.hpp"
#include "UI/UIManager.hpp"
#include "UIAxisGizmo.hpp"
#include "UITransform.hpp"
#include "UISceneTree.hpp"

class Editor: public Script
{
public:
    virtual void init() override;
    virtual void firstUpdate(Core::f32 dt) override;
    virtual void update(Core::f32 dt) override;
    virtual void terminate() override;

private:
	GameObject* createCube(const Maths::Vector3& v, Core::f32 size);
	GameObject* createSprite(const Maths::Vector3& v, Core::f32 size);
	GameObject* createPointLight(const Maths::Vector3& v, Core::f32 size);
	GameObject* createDirectionalLight(const Maths::Vector3& v, const Maths::Vector3& dir);
	GameObject* mousePick();
    GameObject* importModel(const std::string& pFile, const Maths::Vector3& v, Core::f32 size, const Maths::Vector3& rot, bool isStatic);
	void handlePressedKeys();
	void handleMouse();
    void createUI();
private:
    UIText* mFPSCounter = nullptr;
    UITransform* mUITransform = nullptr;
    UISceneTree* mUISceneTree = nullptr;
	GameObject* mCameraGameObject = nullptr;
	GameObject* mDirectionalLight = nullptr;
	GameObject* mSelectedGameObject = nullptr;
	std::list<GameObject*> mBuildings;
	UIAxisGizmo* mAxisViewer = nullptr;

    std::vector<GameObject*> mGameObjectsArray;

    Maths::Vector2 mLastMousePosition;
	Maths::Vector3 mCurrentRotation;
	Maths::Vector3 mTargetRotation;
};
REGISTER_CLASS(Editor, Script)