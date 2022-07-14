#include "UI/UIElements/UIDropdown.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIElements/UIText.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"


void UIDropdownButton::onPostReleased() 
{
	UIButton::onPostReleased();
	mParentDropdown->setEntriesVisibility(false);
}

UIDropdownEntry::UIDropdownEntry(CR(std::string) label, UIElementCallback callback)
{
	mLabel = label;
	mCallback = callback;
}

void UIDropdown::init() 
{
	UIButton::init();
}

void UIDropdown::onDestroy() 
{
	UIButton::onDestroy();
}

void UIDropdown::onPostReleased() 
{
	UIButton::onPostReleased();
	toggle();
}

R(UIDropdown) UIDropdown::addOption(CR(std::string) label, UIElementCallback onPressedCallback)
{
	mEntries.push_back(UIDropdownEntry(label, onPressedCallback));
	return *this;
}

void UIDropdown::toggle()
{
	setEntriesVisibility(mButtons.empty() ? true : !mButtons.front()->isActive());
}

void UIDropdown::setEntriesVisibility(bool visible)
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
				setPosition(getTransform().get().getWorldPosition() + Vector2((-scale.x / 2.0f) / RenderContext::getAspectRatio(), -scale.y / 2.0f)).
				setTextSize(mConfig.mTextSize).
				setAdjustSizeToText(true).
				setLayer(mConfig.mLayer);

			FOR_LIST(it, mEntries)
			{
				R(std::string) label = (*it).mLabel;
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
