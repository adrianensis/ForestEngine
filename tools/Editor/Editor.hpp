#pragma once

#include "Scripting/Script.hpp"
#include "UI/UIStyle.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class UIStyleEditorToolButton: public UIStyleDefault
{
    GENERATE_METADATA(UIStyleEditorToolButton)

public:
	UIStyleEditorToolButton()
	{
		mBackgroundColor = Vector4(0.0f, 0.0f, 0.0f, 1);
		mColorPressed = Vector4(-0.3f, -0.3f, -0.3f, 1);
		mColorHovered = Vector4(0.4f, 0.4f, 0.4f, 1);
	}
};

class Editor: public Script
{
    GENERATE_METADATA(Editor)

public:
    void init();
    void firstUpdate();
    void update();
    void terminate();

private:
	Ptr<GameObject> createSprite(const Vector3& v, f32 size);
    Ptr<GameObject> importModel(const std::string& pFile, const Vector3& v, f32 size, f32 rot);
    Ptr<GameObject> importModel2(const std::string& pFile, const Vector3& v, f32 size, f32 rot);
	void handlePressedKeys();
private:
	f32 rotation = 0;
	f32 y = 0;
	Vector3 position;
    Ptr<GameObject> gameObject;
	Ptr<GameObject> cameraGameObject;

	Vector2 mousePosition;
};
