#pragma once

#include "UI/UIButton.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIDropdown.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIText.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"
#endif

class UIDropdown;

class UIDropdownButton: public UIButton
{
    GENERATE_METADATA(UIDropdownButton)

private:	
	UIDropdown* mParentDropdown = nullptr;

public:
	CPP void onPostReleased() override
	{
		UIButton::onPostReleased();
		mParentDropdown->setEntriesVisibility(false);
	}

	GET_SET(ParentDropdown)
};

class UIDropdownEntry: public ObjectBase
{
	GENERATE_METADATA(UIDropdownEntry)

public:
	std::string mLabel;
	UIElementCallback mCallback;

	UIDropdownEntry() = default;
	
	CPP UIDropdownEntry(const std::string& label, UIElementCallback callback)
	{
		mLabel = label;
		mCallback = callback;
	}

	COPY(UIDropdownEntry)
	{
		DO_COPY(mLabel)
		DO_COPY(mCallback)
	}
};

class UIDropdown: public UIButton
{
    GENERATE_METADATA(UIDropdown)
	friend UIDropdownButton;

private:

	std::list<UIDropdownButton *> mButtons;
	std::list<UIDropdownEntry> mEntries;

	CPP void setEntriesVisibility(bool visible)
	{
		if(visible)
		{
			if (mButtons.empty())
			{
				Vector3 scale = getTransform().get().getScale();
				scale.x = scale.x * RenderContext::getAspectRatio();
				
				UIBuilder uiBuilder;

				uiBuilder.
					setLayout(UILayout::VERTICAL).
					//setSize(scale).
					setPosition(getTransform().get().getWorldPosition() + Vector2((-scale.x / 2.0f) / RenderContext::getAspectRatio(), -scale.y / 2.0f)).
					setTextSize(mConfig.mTextSize).
					setAdjustSizeToText(true).
					setLayer(mConfig.mLayer);

				FOR_LIST(it, mEntries)
				{
					std::string& label = (*it).mLabel;
					UIElementCallback onPressedCallback = (*it).mCallback;

					uiBuilder.
					setText(label).
					create<UIDropdownButton>();

					UIDropdownButton *button = uiBuilder.getUIElement<UIDropdownButton>();
					button->setOnPressedCallback(onPressedCallback);

					button->setParentDropdown(this);

					mButtons.push_back(button);
				}
			}
			
			FOR_LIST(it, mButtons)
			{
				(*it)->setVisibility(true);
			}
		}
		else
		{
			if(!mButtons.empty())
			{
				FOR_LIST(it, mButtons)
				{
					getScene()->removeGameObject(*it);
				}

				mButtons.clear();
			}
		}
	}

public:
	CPP void init() override
	{
		UIButton::init();
	}

	CPP void onDestroy() override
	{
		UIButton::onDestroy();
	}

	CPP void onPostReleased() override
	{
		UIButton::onPostReleased();
		toggle();
	}

	CPP UIDropdown& addOption(const std::string& label, UIElementCallback onPressedCallback)
	{
		mEntries.push_back(UIDropdownEntry(label, onPressedCallback));
		return *this;
	}

	CPP void toggle()
	{
		// TODO : If I want to create-remove buttons, I have to implement TIMER NEXT FRAME!
		setEntriesVisibility(mButtons.empty() ? true : !mButtons.front()->isActive());
	}
};