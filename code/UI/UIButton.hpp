#pragma once

#include "UI/UIElement.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIButton.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIBuilder.hpp"
#include "UI/UIText.hpp"
#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "Scene/Scene.hpp"
#endif

class UIText;

class UIButton: public UIElement
{
    GENERATE_METADATA(UIButton)
	
private:
	UIText* mText = nullptr;

public:
	CPP void init() override
	{
		UIElement::init();

		subscribeToMouseEvents();
	}

	CPP void initFromConfig(const UIElementConfig& config) override
	{
		UIElement::initFromConfig(config);

		getTransform().get().setLocalPosition(mConfig.mDisplayPosition);
		getTransform().get().setScale(Vector3(UIUtils::correctAspectRatio_X(mConfig.mSize), 1));
		getTransform().get().setAffectedByProjection(false);

		Renderer *renderer = NEW(Renderer);
		renderer->init();

		renderer->setMesh(MeshPrimitives::getInstance().getPrimitive<Rectangle>());
		renderer->setMaterial(mConfig.mMaterial);
		renderer->setDepth(mConfig.mLayer);
		renderer->setColor(mConfig.mStyle->mBackgroundColor);

		addComponent<Renderer>(renderer);
		
		setComponentsCache();

		setText(mConfig.mText);
	}

	CPP void onDestroy() override
	{
		if (mText)
		{
			getScene()->removeGameObject(mText);
			mText = nullptr;
		}

		UIElement::onDestroy();
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
		UIElement::setVisibility(visibility);

		if (mText)
		{
			mText->setVisibility(visibility);
		}
	}

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