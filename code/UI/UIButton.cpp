#include "UI/UIButton.hpp"
#include "UI/UI.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIText.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/RenderContext.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"
#include "Events/EventsManager.hpp"

void UIButton::init()
{
	UIElement::init();

	subscribeToMouseButtonEvents();
}

void UIButton::onDestroy()
{
	if (mLabel)
	{
		getScene()->removeGameObject(mLabel);
	}

	UIElement::onDestroy();
}

void UIButton::onLabelDestroy()
{
	mLabel = nullptr;
}

void UIButton::setText(const std::string &text)
{
	if (text.length() > 0)
	{
		f32 fontOffset = UI::getInstance()->getDefaultFontSize().x;
		Vector3 buttonScale = getTransform()->getScale();

		if (!mLabel)
		{
			mLabel = (UIText *)UI::getInstance()->getUIBuilder().
			saveData().
			setPosition(Vector2(0, 0)).
			setText(text).
			setIsAffectedByLayout(false).
			setParent(this).
			create(UIElementType::TEXT).
			getUIElement();

			UI::getInstance()->getUIBuilder().restoreData();

			SUBSCRIBE_TO_EVENT(EventOnDestroy, mLabel, this, [&](const Event *event)
			{
				onLabelDestroy();
			});
		}

		mLabel->setText(text);
	}
}

void UIButton::setVisibility(bool visibility)
{
	UIElement::setVisibility(visibility);

	if (mLabel)
	{
		mLabel->setVisibility(visibility);
	}
}