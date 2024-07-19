#include "UI/UIElements/UIButton.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIElements/UIText.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"


void UIButton::init() 
{
	UIPanel::init();

	subscribeToMouseEvents();
}

void UIButton::initFromConfig(const UIElementConfig& config) 
{
	UIPanel::initFromConfig(config);

	setText(mConfig.mText);
}

void UIButton::onDestroy() 
{
	if (mText)
	{
		mScene->removeGameObject(mText);
		mText.reset();
	}

	UIPanel::onDestroy();
}

void UIButton::onLabelDestroy()
{
	mText.reset();
}

void UIButton::setText(HashedString text)
{
	if (text.get().length() > 0)
	{
		if (!mText)
		{
			UIBuilder uiBuilder;

			mText = uiBuilder.
			setPosition(Vector2(-mConfig.mDisplaySize.x/2.0f, mConfig.mDisplaySize.y/2.0f)).
			setSize(mConfig.mDisplaySize).
			setText(text).
			setTextScale(mConfig.mTextScale).
			setLayer(mConfig.mLayer + 1).
			setIsAffectedByLayout(false).
			setParent(EntityHandler::getEntityHandler(*this)).
			create<UIText>().
			getUIElement<UIText>();
		}

		mText->setText(text);
	}
}

void UIButton::setVisibility(bool visibility) 
{
	UIPanel::setVisibility(visibility);

	if (mText)
	{
		mText->setVisibility(visibility);
	}
}