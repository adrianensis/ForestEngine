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
	const UIStyle* mStyle = nullptr;
	ClassId mUIElementClassId = 0;
	Vector2 mPosition = Vector2::smZero;
	Vector2 mSize = Vector2::smOne;
	Vector2 mDisplayPosition = Vector2::smZero;
	Vector2 mDisplaySize = Vector2::smOne;
	bool mIsStatic = true;
    bool mToggleEnabled = false;
	bool mReleaseOnSameGroupPressed = false;
	// Vector2 mTextScale;
	HashedString mText;
	f32 mTextScale = 1.0f;
	bool mAdjustSizeToText = false;
	i32 mLayer = 0;
	bool mIsAffectedByLayout = true;
	PoolHandler<Material> mMaterial;
	HashedString mGroup;
	Ptr<GameObject> mParent;
	f32 mSeparatorSize = 0.01f;
};
