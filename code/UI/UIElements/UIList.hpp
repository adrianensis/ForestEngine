#pragma once

#include "UI/UIElements/UIElement.hpp"

class UIButton;

class UIListEntry: public ObjectBase
{
	GENERATE_METADATA(UIListEntry)

public:
	std::string mLabel;
	UIElementCallback mCallback;
	
	void init(const std::string& label, UIElementCallback callback);

	COPY(UIListEntry)
	{
		DO_COPY(mLabel)
		DO_COPY(mCallback)
	}
};

class UIList: public UIElement
{
    GENERATE_METADATA(UIList)

private:

	std::list<UIButton *> mButtons;
	std::list<UIListEntry> mEntries;

	void setEntriesVisibility(bool visible);

public:
	void init() override;
	void initFromConfig(const UIElementConfig& config) override;
	void onDestroy() override;

	UIList& addOption(const std::string & label, UIElementCallback onPressedCallback);

	void toggle();
	void onScroll(f32 scroll) override;
};