#pragma once

#include "UI/UIButton.hpp"


class UIList: public UIElement
{
    GENERATE_METADATA(UIList)

private:
	class UIListEntry: public ObjectBase
	{
		GENERATE_METADATA(UIList::UIListEntry)

	public:
		std::string mLabel;
		UIElementCallback mCallback;
		
		UIListEntry(const std::string& label, UIElementCallback callback);

		COPY(UIListEntry)
		{
			DO_COPY(mLabel)
			DO_COPY(mCallback)
		}
	};

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