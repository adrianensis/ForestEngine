#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

class Material;
class Scene;
class GameObject;
class UIStyle;

enum class UILayout
{
	HORIZONTAL,
	VERTICAL
};

class UIElementConfig
{
public:
    void init(const Vector2& position, const Vector2& size, i32 layer, PoolHandler<Material> material, ConstString text = ConstString());

public:
	const UIStyle* mStyle = nullptr;
	ClassId mUIElementClassId;
	Vector2 mPosition = Vector2::smZero;
	Vector2 mSize = Vector2::smOne;
	Vector2 mDisplayPosition = Vector2::smZero;
	Vector2 mDisplaySize = Vector2::smOne;
	bool mIsStatic = true;
    bool mToggleEnabled = false;
	bool mReleaseOnSameGroupPressed = false;
	// Vector2 mTextSize;
	ConstString mText;
	bool mAdjustSizeToText = false;
	i32 mLayer = 0;
	bool mIsAffectedByLayout = true;
	PoolHandler<Material> mMaterial;
	ConstString mGroup;
	Ptr<GameObject> mParent;
	f32 mSeparatorSize = 0.0f;
};
