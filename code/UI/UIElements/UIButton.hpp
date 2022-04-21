#pragma once

#include "UI/UIElements/UIPanel.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIElements/UIButton.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIElements/UIText.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"
#endif

class UIText;

class UIButton: public UIPanel
{
    GENERATE_METADATA(UIButton)
	
public:
	CPP void init() override
	{
		UIPanel::init();

		subscribeToMouseEvents();
	}

	CPP void initFromConfig(const UIElementConfig& config) override
	{
		UIPanel::initFromConfig(config);

		setText(mConfig.mText);
	}

	CPP void onDestroy() override
	{
		if (mText)
		{
			getScene()->removeGameObject(mText);
			mText = nullptr;
		}

		UIPanel::onDestroy();
	}

	CPP void onLabelDestroy()
	{
		mText = nullptr;
	}

	CPP void setText(const std::string& text) override
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
				create<UIText>().
				getUIElement<UIText>();
			}

			mText->setText(text);
		}
	}

	CPP void setVisibility(bool visibility) override
	{
		UIPanel::setVisibility(visibility);

		if (mText)
		{
			mText->setVisibility(visibility);
		}
	}

private:
	UIText* mText = nullptr;

public:
	GET(Text)
};

class UIToggleButton: public UIButton
{
    GENERATE_METADATA(UIToggleButton)
public:
	CPP void init() override
	{
		UIButton::init();

		mCanToggle = true;
		mReleaseOnSameGroupPressed = true;
	}
};