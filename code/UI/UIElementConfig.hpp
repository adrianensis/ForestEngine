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
    void init(const Vector2& position, const Vector2& size, i32 layer, PoolHandler<Material> material, std::string text = std::string());

public:
	const UIStyle* mStyle = nullptr;
	ClassId mUIElementClassId;
	Vector2 mPosition = Vector2::smZero;
	Vector2 mSize = Vector2::smOne;
	Vector2 mDisplayPosition = Vector2::smZero;
	Vector2 mDisplaySize = Vector2::smOne;
	bool mIsStatic = true;
	// Vector2 mTextSize;
	std::string mText;
	bool mAdjustSizeToText = false;
	i32 mLayer = 0;
	bool mIsAffectedByLayout = true;
	PoolHandler<Material> mMaterial;
	std::string mGroup;
	Ptr<GameObject> mParent;
	f32 mSeparatorSize = 0.0f;
};
