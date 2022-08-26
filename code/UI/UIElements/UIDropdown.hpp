#ifndef UIDROPDOWN_HPP
#define UIDROPDOWN_HPP

#include "UI/UIElements/UIButton.hpp"


class UIDropdown;

class UIDropdownButton: public UIButton
{
    GENERATE_METADATA(UIDropdownButton)

public:
    void onPostReleased() override;

private:	
	UIDropdown* mParentDropdown = nullptr;
public:
	GET_SET(ParentDropdown)
};

class UIDropdownEntry: public ObjectBase
{
	GENERATE_METADATA(UIDropdownEntry)

public:
	UIDropdownEntry() = default;
	
    UIDropdownEntry(const std::string& label, UIElementCallback callback);

	COPY(UIDropdownEntry)
	{
		DO_COPY(mLabel)
		DO_COPY(mCallback)
	}

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
	std::list<UIDropdownButton *> mButtons;
	std::list<UIDropdownEntry> mEntries;

};

#endif