#pragma once

#include "UI/UIElements/UIButton.hpp"


class UIDropdown;

class UIDropdownButton: public UIButton
{
    GENERATE_METADATA(UIDropdownButton)

public:
    void onPostReleased() override;

private:
    Ptr<UIDropdown> mParentDropdown;
public:
	GET_SET(ParentDropdown)
};

class UIDropdownEntry: public ObjectBase
{
	GENERATE_METADATA(UIDropdownEntry)

public:
	UIDropdownEntry() = default;
    UIDropdownEntry(const std::string& label, UIElementCallback callback);

public:
	std::string mLabel;
	UIElementCallback mCallback;
};

class UIDropdown: public UIButton
{
    GENERATE_METADATA(UIDropdown)
	friend UIDropdownButton;
public:
    void init() override;
    void onDestroy() override;
    void onPostReleased() override;
    UIDropdown& addOption(const std::string& label, UIElementCallback onPressedCallback);
    void toggle();

private:
    void setEntriesVisibility(bool visible);

private:
	std::list<Ptr<UIDropdownButton>> mButtons;
	std::list<UIDropdownEntry> mEntries;

};
