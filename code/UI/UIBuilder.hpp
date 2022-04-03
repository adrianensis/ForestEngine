#pragma once

#include "Core/Module.hpp"
#include "UI/UIElement.hpp"
#include "UI/UIButton.hpp"
#include "UI/UIEditableText.hpp"
#include "UI/UIDropdown.hpp"
#include "UI/UIText.hpp"
#include "UI/UIPanel.hpp"
//#include "UI/UIList.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIStyle.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Scene/Module.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIBuilder.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIStyle.hpp"
#endif

#define UI_BUILDER_CONFIG_SETTER(Name)   \
	UIBuilder& set##Name(SETTER_TYPE_FROM_VAR(mConfig.m##Name) new##Name) \
	{                                              \
		mConfig.m##Name = new##Name;                   \
		return *this;                               \
	}

#define UI_BUILDER_CONFIG_RESTORE(Name)   \
	UIBuilder& restore##Name() \
	{                                              \
		mConfig.m##Name = mDefaultConfig.m##Name;                   \
		return *this;                               \
	}

#define UI_BUILDER_CONFIG_METHODS(Name)\
	UI_BUILDER_CONFIG_RESTORE(Name)\
	UI_BUILDER_CONFIG_SETTER(Name)

class UIBuilder: public ObjectBase
{
    GENERATE_METADATA(UIBuilder)
	
private:
	UILayout mCurrentLayout;
	UIElementConfig mConfig;
	UIElementConfig mDefaultConfig;
	std::list<UIElementConfig> mConfigStack;
	UIElementConfig mLastConfig;
	bool mMakeRelativeToLastConfig = false; // used for layouts
	UIElementConfig mLayoutFirstUIElementConfig;
	bool mNewRowOrColumn = false;
	UIElement* mCurrentUIElement = nullptr;

private:
	CPP void registerUIElement(UIElement *uiElement)
	{
		mCurrentUIElement = uiElement;

		ScenesManager::getInstance().getCurrentScene()->addGameObject(mCurrentUIElement);

		if (mConfig.mGroup.length() > 0)
		{
			UIManager::getInstance().getOrCreateGroup(mConfig.mGroup).addUIElement(mCurrentUIElement);
		}

		if (mConfig.mIsAffectedByLayout)
		{
			if (mNewRowOrColumn)
			{
				mNewRowOrColumn = false;
				mLayoutFirstUIElementConfig = mConfig;
			}

			mMakeRelativeToLastConfig = true;

			mLastConfig = mConfig;
		}
	}

	CPP UILayout getOppositeLayout(UILayout layout)
	{
		return (UILayout)(((int)mCurrentLayout + 1) % 2);
	}

	CPP Vector2 calculateNextElementOffset(UILayout layout)
	{
		Vector2 offset = Vector2(0, 0);

		switch (layout)
		{
			case UILayout::HORIZONTAL:
			{
				offset = Vector2(mLastConfig.mSize.x / RenderContext::getAspectRatio() + mConfig.mSeparatorSize, 0);
				break;
			}
			case UILayout::VERTICAL:
			{
				offset = Vector2(0, -(mLastConfig.mSize.y + mConfig.mSeparatorSize));
				break;
			}
		}

		return offset;
	}

	CPP void calculateConfig()
	{
		if (mConfig.mAdjustSizeToText)
		{
			mConfig.mSize.x = (mConfig.mTextSize.x* mConfig.mText.length());
			mConfig.mSize.y = mConfig.mTextSize.y;
		}

		if (mConfig.mIsAffectedByLayout && mMakeRelativeToLastConfig)
		{
			Vector2 offset = calculateNextElementOffset(mNewRowOrColumn ? getOppositeLayout(mCurrentLayout) : mCurrentLayout);
			mConfig.mPosition = mLastConfig.mPosition + offset;
		}

		// Offset the UI Element so its Top-Left corner is the origin.
		mConfig.mDisplayPosition = mConfig.mPosition;

		if(mConfig.mUIElementClassId == UIText::getClassIdStatic() || mConfig.mUIElementClassId == UIEditableText::getClassIdStatic())
		{
			mConfig.mDisplayPosition.x += mConfig.mTextSize.x/RenderContext::getAspectRatio();
			mConfig.mDisplayPosition.y -= mConfig.mTextSize.y / 2.0f;
		}
		else
		{
			mConfig.mDisplayPosition.x += (mConfig.mSize.x/RenderContext::getAspectRatio()) / 2.0f;
			mConfig.mDisplayPosition.y -= mConfig.mSize.y / 2.0f;
		}
	}

public:
	CPP UIBuilder()
	{
		mCurrentLayout = UILayout::VERTICAL;
		mMakeRelativeToLastConfig = false;
		mCurrentUIElement = nullptr;

		mDefaultConfig.init(Vector2(0, 0), Vector2(0, 0), 0);
		mConfig = mDefaultConfig;
		//mSavedData.init(Vector2(0,0), Vector2(0,0), "", 0);
	}

	UI_BUILDER_CONFIG_METHODS(IsAffectedByLayout)
	UI_BUILDER_CONFIG_METHODS(Position)
	UI_BUILDER_CONFIG_METHODS(Size)
	UI_BUILDER_CONFIG_METHODS(Layer)
	UI_BUILDER_CONFIG_METHODS(Text)
	UI_BUILDER_CONFIG_METHODS(TextSize)
	UI_BUILDER_CONFIG_METHODS(AdjustSizeToText)
	UI_BUILDER_CONFIG_METHODS(Group)
	UI_BUILDER_CONFIG_METHODS(SeparatorSize)
	UI_BUILDER_CONFIG_METHODS(Parent)
	UI_BUILDER_CONFIG_METHODS(Style)
	UI_BUILDER_CONFIG_METHODS(Material)

	void restoreAll() { mConfig = mDefaultConfig; }

	UIBuilder& setLayout(UILayout layout)
	{
		mCurrentLayout = layout;
		mMakeRelativeToLastConfig = false; // reset
		mNewRowOrColumn = true;
		return *this;
	}

	CPP UIBuilder& nextRow()
	{
		mLastConfig = mLayoutFirstUIElementConfig;
		mNewRowOrColumn = true;
		return *this;
	}

	CPP UIBuilder& nextColumn()
	{
		return nextRow(); // NOTE : exactly the same code.
	}

	CPP UIBuilder& saveData()
	{
		mConfigStack.push_front(mConfig);
		return *this;
	}

	CPP UIBuilder& restoreData()
	{
		mConfig = mConfigStack.front();
		mConfigStack.pop_front();
		return *this;
	}

	CPP UIBuilder& create(const std::string& className)
	{
		UIElement* uiElement = INSTANCE_BY_NAME(className, UIElement);
		mConfig.mUIElementClassId = uiElement->getClassId();

		calculateConfig();
		uiElement->initFromConfig(mConfig);

		registerUIElement(uiElement);

		return *this;
	}

	template<class T, typename = std::enable_if_t<std::is_base_of<UIElement, T>::value> >
	UIBuilder& create()
	{
		return create(T::getClassNameStatic());
	}

	UIElement *getUIElement() const
	{
		return mCurrentUIElement;
	}

	template<class T, typename = std::enable_if_t<std::is_base_of<UIElement, T>::value> >
	T *getUIElement() const
	{
		return dynamic_cast<T *>(getUIElement());
	}

	CRGET_SET(Config)
};