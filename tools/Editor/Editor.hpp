#pragma once

#include "Scripting/Script.hpp"
#include "UI/Module.hpp"

class Editor: public Script
{
public:
    void init();
    void firstUpdate();
    void update();
    void terminate();

private:
	Ptr<GameObject> createSprite(const Vector3& v, f32 size);
	Ptr<GameObject> mousePick();
    Ptr<GameObject> importModel(const std::string& pFile, const Vector3& v, f32 size, f32 rot);
    Ptr<GameObject> importModel2(const std::string& pFile, const Vector3& v, f32 size, f32 rot);
	void handlePressedKeys();
	void handleMouse();
    void createUI();
private:
    Ptr<UIText> fpsCounter;
	Ptr<GameObject> mCameraGameObject;
	Ptr<GameObject> mSelectedGameObject;

    std::vector<Ptr<GameObject>> mGameObjectsArray;

    Vector2 mLastMousePosition;
};
REGISTER_CLASS(Editor)