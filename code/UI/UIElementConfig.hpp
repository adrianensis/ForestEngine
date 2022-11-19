#ifndef UIELEMENTCONFIG_HPP
#define UIELEMENTCONFIG_HPP

#include "Core/Module.hpp"


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
    void init(const Vector2& position, const Vector2& size, i32 layer, std::string text = std::string());

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
		DO_COPY(mStencilValue)
		DO_COPY(mStencilFunction)

		if (!other->mText.empty())
		{
			mText = other->mText;
		}
		if (!other->mGroup.empty())
		{
			mGroup = other->mGroup;
		}
	}

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
	u32 mStencilValue = 0x00;
	u32 mStencilFunction = GL_NOTEQUAL;
};

#endif