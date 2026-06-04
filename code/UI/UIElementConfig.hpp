#pragma once

#include "Engine/Core.hpp"
#include "Scene/ScenesManager.hpp"
#include "GPU/Shader/GPUShader.hpp"

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
	Core::ClassId mUIElementClassId = 0;
	Maths::Vector2 mPosition = Maths::Vector2::smZero;
	Maths::Vector2 mSize = Maths::Vector2::smOne;
	Maths::Vector2 mDisplayPosition = Maths::Vector2::smZero;
	Maths::Vector2 mDisplaySize = Maths::Vector2::smOne;
	bool mIsStatic = true;
	bool mIsStaticText = true;
    bool mToggleEnabled = false;
	bool mReleaseOnSameGroupPressed = false;
	// Maths::Vector2 mTextScale;
	Core::HashedString mText;
	Core::f32 mTextScale = 1.0f;
	bool mAdjustSizeToText = false;
	Core::i32 mLayer = 0;
	bool mIsAffectedByLayout = true;
	GPUShader* mShader = nullptr;
	Core::HashedString mGroup;
	GameObject* mParent = nullptr;
	Core::f32 mSeparatorSize = 0.01f;
};
