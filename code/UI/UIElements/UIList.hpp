#pragma once

#include "UI/UIElements/UIElement.hpp"

class UIButton;

class UIListEntry
{
public:
	std::string mLabel;
	UIElementCallback mCallback;
	
	void init(const std::string& label, UIElementCallback callback);
};

class UIList: public UIElement
{
    
    
public:
	void init() override;
	void initFromConfig(const UIElementConfig& config) override;
	void onDestroy() override;

	UIList& addOption(const std::string & label, UIElementCallback onPressedCallback);

	void toggle();
	void onScroll(f32 scroll) override;

private:
	void setEntriesVisibility(bool visible);

private:
	std::list<TypedEntityHandler<UIButton>> mButtons;
	std::list<UIListEntry> mEntries;

};
REGISTER_CLASS(UIList);
