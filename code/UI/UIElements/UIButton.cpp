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
		getScene()->removeGameObject(mText);
		mText = nullptr;
	}

	UIPanel::onDestroy();
}

void UIButton::onLabelDestroy()
{
	mText = nullptr;
}

void UIButton::setText(const std::string& text) 
{
	if (text.length() > 0)
	{
		Vector3 buttonScale = getTransform().get().getScale();

		if (!mText)
		{
			UIBuilder uiBuilder;

			mText = uiBuilder.
			setPosition(Vector2(0, 0)).
			setText(text).
			setLayer(mConfig.mLayer - 1).
			setIsAffectedByLayout(false).
			setParent(this).
            setStencilValue(mConfig.mStencilValue).
            setStencilFunction(mConfig.mStencilFunction).
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

void UIToggleButton::init() 
{
	UIButton::init();

	mToggleEnabled = true;
	mReleaseOnSameGroupPressed = true;
}
