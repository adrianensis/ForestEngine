#pragma once

#include "Core/Minimal.hpp"
#include "UI/UIElements/UIElement.hpp"
#include "UI/UIElements/UIButton.hpp"
#include "UI/UIElements/UIEditableText.hpp"
#include "UI/UIElements/UIDropdown.hpp"
#include "UI/UIElements/UIText.hpp"
#include "UI/UIElements/UIPanel.hpp"
#include "UI/UIElementConfig.hpp"
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
        PROFILER_CPU()
        mConfig.mUIElementClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();

        calculateConfig();
	    TypedEntityHandler<T> uiElement = GET_SYSTEM(ScenesManager).getScene(mConfig.mSceneName)->createGameObject<T>();
        uiElement->initFromConfig(mConfig);
        uiElement->postInit();

        registerUIElement(uiElement);
		return *this;
	}

    TypedEntityHandler<UIElement> getUIElement() const
	{
		return mCurrentUIElement;
	}

	template<class T> T_EXTENDS(T, UIElement)
    TypedEntityHandler<T> getUIElement() const
	{
		return getUIElement();
	}

private:
    void registerUIElement(TypedEntityHandler<UIElement> uiElement);
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
	TypedEntityHandler<UIElement> mCurrentUIElement;

public:

	UI_BUILDER_CONFIG_METHODS(IsAffectedByLayout)
	UI_BUILDER_CONFIG_METHODS(Position)
	UI_BUILDER_CONFIG_METHODS(Size)
	UI_BUILDER_CONFIG_METHODS(IsStatic)
	UI_BUILDER_CONFIG_METHODS(IsStaticText)
	UI_BUILDER_CONFIG_METHODS(SceneName)
	UI_BUILDER_CONFIG_METHODS(ToggleEnabled)
	UI_BUILDER_CONFIG_METHODS(ReleaseOnSameGroupPressed)
	UI_BUILDER_CONFIG_METHODS(Layer)
	UI_BUILDER_CONFIG_METHODS(Text)
	UI_BUILDER_CONFIG_METHODS(TextScale)
	// UI_BUILDER_CONFIG_METHODS(TextScale)
	UI_BUILDER_CONFIG_METHODS(AdjustSizeToText)
	UI_BUILDER_CONFIG_METHODS(Group)
	UI_BUILDER_CONFIG_METHODS(SeparatorSize)
	UI_BUILDER_CONFIG_METHODS(Parent)
	UI_BUILDER_CONFIG_METHODS(Style)
	UI_BUILDER_CONFIG_METHODS(Material)

	CRGET_SET(Config)
};
