#include "UI/UIElements/UIDropdown.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIElements/UIText.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"


void UIDropdownButton::onPostPressed() 
{
	UIButton::onPostPressed();
	mParentDropdown->setEntriesVisibility(false);
}

UIDropdownEntry::UIDropdownEntry(const std::string& label, UIElementCallback callback)
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

void UIDropdown::onPostPressed() 
{
	UIButton::onPostPressed();
	toggle();
}

UIDropdown& UIDropdown::addOption(const std::string& label, UIElementCallback onPressedCallback)
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
			Vector3 scale = mTransform->getLocalScale();
			scale.x = scale.x * GET_SYSTEM(WindowManager).getMainWindow()->getAspectRatio();
			
			UIBuilder uiBuilder;

			uiBuilder.
				setLayout(UILayout::VERTICAL).
				setPosition(mTransform->getWorldPosition() + Vector2((-scale.x / 2.0f) / GET_SYSTEM(WindowManager).getMainWindow()->getAspectRatio(), -scale.y / 2.0f)).
				setTextScale(mConfig.mTextScale).
				setAdjustSizeToText(true).
				setLayer(mConfig.mLayer);

			FOR_LIST(it, mEntries)
			{
				HashedString label = HashedString((*it).mLabel);
				UIElementCallback onPressedCallback = (*it).mCallback;

				uiBuilder.
				setText(label).
				create<UIDropdownButton>();

				TypedEntityHandler<UIDropdownButton> button = uiBuilder.getUIElement<UIDropdownButton>();
				button->setOnPressedCallback(onPressedCallback);

				button->setParentDropdown(EntityHandler::getEntityHandler(*this));

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
				mScene->removeGameObject(*it);
			}

			mButtons.clear();
		}
	}
}
