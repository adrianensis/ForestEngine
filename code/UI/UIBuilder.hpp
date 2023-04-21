#pragma once

#include "Core/Module.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "UI/UIElements/UIButton.hpp"
#include "UI/UIElements/UIEditableText.hpp"
#include "UI/UIElements/UIDropdown.hpp"
#include "UI/UIElements/UIText.hpp"
#include "UI/UIElements/UIPanel.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIStyle.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Scene/Module.hpp"


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

class UIBuilder
{
public:
    UIBuilder();

	void restoreAll() { mConfig = mDefaultConfig; }

	UIBuilder& setLayout(UILayout layout)
	{
		mCurrentLayout = layout;
		mMakeRelativeToLastConfig = false; // reset
		mNewRowOrColumn = true;
		return *this;
	}

    UIBuilder& nextRow();
    UIBuilder& nextColumn();
    UIBuilder& saveData();
    UIBuilder& restoreData();

	template<class T> T_EXTENDS(T, UIElement)
	UIBuilder& create()
	{
        OwnerPtr<UIElement> uiElement = OwnerPtr<UIElement>::cast(OwnerPtr<T>::newObject());
        mConfig.mUIElementClassId = uiElement->getClassId();

        calculateConfig();
        uiElement->initFromConfig(mConfig);

        registerUIElement(uiElement);
		return *this;
	}

    Ptr<UIElement> getUIElement() const
	{
		return mCurrentUIElement;
	}

	template<class T> T_EXTENDS(T, UIElement)
    Ptr<T> getUIElement() const
	{
		return Ptr<T>::cast(getUIElement());
	}

private:
    void registerUIElement(OwnerPtr<UIElement> uiElement);
    UILayout getOppositeLayout(UILayout layout);
    Vector2 calculateNextElementOffset(UILayout layout);
    void calculateConfig();

private:
	UILayout mCurrentLayout;
	UIElementConfig mConfig;
	UIElementConfig mDefaultConfig;
	std::list<UIElementConfig> mConfigStack;
	UIElementConfig mLastConfig;
	bool mMakeRelativeToLastConfig = false; // used for layouts
	UIElementConfig mLayoutFirstUIElementConfig;
	bool mNewRowOrColumn = false;
	OwnerPtr<UIElement> mCurrentUIElement;

public:

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
	UI_BUILDER_CONFIG_METHODS(StencilValue)
	UI_BUILDER_CONFIG_METHODS(StencilFunction)

	CRGET_SET(Config)
};
