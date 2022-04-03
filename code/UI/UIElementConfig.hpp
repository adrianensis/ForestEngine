#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIElementConfig.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"
#include "Graphics/Module.hpp"
#endif

class Material;
class Scene;
class GameObject;
class UIStyle;

enum class UILayout
{
	HORIZONTAL,
	VERTICAL
};

class UIElementConfig: public ObjectBase
{
    GENERATE_METADATA(UIElementConfig)

public:
	const UIStyle* mStyle = nullptr;
	ClassId mUIElementClassId;
	Vector2 mPosition;
	Vector2 mDisplayPosition;
	Vector2 mSize;
	Vector2 mTextSize;
	std::string mText;
	bool mAdjustSizeToText = false;
	i32 mLayer = 0;
	bool mIsAffectedByLayout = true;
	Ptr<Material> mMaterial;
	std::string mGroup;
	GameObject* mParent = nullptr;
	f32 mSeparatorSize = 0.0f;

	CPP void init(const Vector2& position, const Vector2& size, i32 layer, std::string text = std::string())
	{
		mStyle = &UIStyleManager::getInstance().getDefaultStyle();
		mUIElementClassId = 0;
		mPosition = position;
		mDisplayPosition = Vector2(0, 0);
		mSize = size;
		mText = text;
		mLayer = layer;
		mTextSize = UIManager::getDefaultFontSize(); // TODO : move to config?
		mAdjustSizeToText = false;
		mIsAffectedByLayout = true;
		mMaterial = MaterialManager::getInstance().loadNoTextureMaterial();
		mGroup = "";
		mParent = nullptr;
		mSeparatorSize = 0.01f;
	}

	COPY(UIElementConfig)
	{
		DO_COPY(mStyle)
		DO_COPY(mUIElementClassId)
		DO_COPY(mPosition)
		DO_COPY(mDisplayPosition)
		DO_COPY(mSize)
		DO_COPY(mTextSize)
		DO_COPY(mAdjustSizeToText)
		DO_COPY(mLayer)
		DO_COPY(mIsAffectedByLayout)
		DO_COPY(mMaterial)
		DO_COPY(mSeparatorSize)
		DO_COPY(mParent)

		if (!other->mText.empty())
		{
			mText = other->mText;
		}
		if (!other->mGroup.empty())
		{
			mGroup = other->mGroup;
		}
	}
};