#pragma once

#include "UI/UIButton.hpp"


class UIDropdown;

class UIDropdownButton: public UIButton
{
    GENERATE_METADATA(UIDropdownButton)
	PRI UIDropdown* mParentDropdown = nullptr; GET_SET(ParentDropdown)
PUB
	void onPostReleased() override;
};

class UIDropdown: public UIButton
{
    GENERATE_METADATA(UIDropdown)
friend UIDropdownButton;

PRI
	class UIDropdownEntry: public ObjectBase
	{
		GENERATE_METADATA(UIDropdown::UIDropdownEntry)
		PUB std::string mLabel;
		PUB UIElementCallback mCallback;

	PUB
		UIDropdownEntry(const std::string& label, UIElementCallback callback);

		COPY(UIDropdownEntry)
		{
			DO_COPY(mLabel)
			DO_COPY(mCallback)
		}
	};

	std::list<UIDropdownButton *> mButtons;
	std::list<UIDropdownEntry> mEntries;

	void setEntriesVisibility(bool visible);

PUB
	void init() override;
	void onDestroy() override;

	void onPostReleased() override;

	UIDropdown& addOption(const std::string & label, UIElementCallback onPressedCallback);
	void toggle();
};