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

class UIBuilder: public ObjectBase
{
    GENERATE_METADATA(UIBuilder)
	
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
    UIBuilder& create(const std::string& className);

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

private:
    void registerUIElement(UIElement *uiElement);
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
	UIElement* mCurrentUIElement = nullptr;

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
