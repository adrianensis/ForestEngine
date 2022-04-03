#pragma once

#include "UI/UIElement.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIText.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIPanel.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Transform.hpp"
#endif

class UIPanel;

class UIText: public UIElement
{
    GENERATE_METADATA(UIText)

private:
	i32 mLayer = 0;
	Vector2 mSize;
	std::string mString;
	std::vector<Renderer *> mFontRenderers;
    bool mIsEditable = false;

protected:
    UIPanel* mBackground = nullptr;
    
	CPP void setIsEditable(bool editable)
	{
		if(editable && !getIsEditable())
		{
			subscribeToMouseEvents();
			subscribeToEnterEvent();
			subscribeToEscEvent();
			subscribeToCharEvents();
			mOnlyReleaseOnClickOutside = true;
		}

		if(!editable && getIsEditable())
		{
			UNSUBSCRIBE_TO_EVENT(InputEventMouseButtonPressed, nullptr, this);
			UNSUBSCRIBE_TO_EVENT(InputEventMouseButtonReleased, nullptr, this);
			UNSUBSCRIBE_TO_EVENT(InputEventChar, nullptr, this);
			UNSUBSCRIBE_TO_EVENT(InputEventKeyBackspace, nullptr, this);
			UNSUBSCRIBE_TO_EVENT(InputEventKeyEnter, nullptr, this);
			UNSUBSCRIBE_TO_EVENT(InputEventKeyEsc, nullptr, this);
			mOnlyReleaseOnClickOutside = true;
		}
	}

    virtual void setBackground(const UIElementConfig& config) { };

public:
	CPP UIText()
	{
		mIsEditable = false;
	}

	CPP void init() override
	{
		UIElement::init();
	}

	CPP void initFromConfig(const UIElementConfig& config) override
	{
		UIElement::initFromConfig(config);

		Vector3 textSize = Vector3(UIUtils::correctAspectRatio_X(mConfig.mTextSize), 1);

		getTransform().get().setLocalPosition(mConfig.mDisplayPosition);
		getTransform().get().setScale(textSize);
		getTransform().get().setAffectedByProjection(false);

		if (mConfig.mParent)
		{
			getTransform().get().setParent(&mConfig.mParent->getTransform().get());
			getTransform().get().setLocalPosition(Vector2(-textSize.x* mConfig.mText.length() / 2.0f + textSize.x,0));
		}

		setSize(mConfig.mTextSize);
		setLayer(mConfig.mLayer);

		setBackground(mConfig);

		setText(mConfig.mText);

		setComponentsCache();
	}

	CPP void onDestroy() override
	{
		UIElement::onDestroy();
	}

	CPP void setText(const std::string& text) override
	{
		if (mString != text)
		{
			bool newRenderersCreated = false;

			if (!mFontRenderers.empty())
			{
				if (text.empty())
				{
					FOR_ARRAY(i, mFontRenderers)
					{
						//removeComponent(mFontRenderers[i]);
					}

					mFontRenderers.clear();
				}
				else
				{
					// Remove Extra Renderers
					if (mFontRenderers.size() > text.length())
					{
						FOR_RANGE(i, text.length(), mString.length())
						{
							//removeComponent(mFontRenderers[i]);
						}

						std::vector<Renderer *> copyVector = mFontRenderers;
						mFontRenderers.clear();
						std::copy(copyVector.begin(), copyVector.begin() + text.length(), std::back_inserter(mFontRenderers));
					}
				}
			}

			if (!text.empty())
			{
				FOR_RANGE(i, 0, text.length())
				{
					Renderer *renderer = nullptr;

					char character = text.at(i);
					Vector2 textureCoordinates = UIManager::getInstance().getCharTextureCoordinates(character);
					Vector2 textureSize = UIManager::getInstance().getFontTileTextureSize();

					if (!mFontRenderers.empty() && i < static_cast<i32>(mString.length()))
					{
						renderer = mFontRenderers[i];
					}
					else
					{
						renderer = NEW(Renderer);
						renderer->init();

						renderer->setMesh(MeshPrimitives::getInstance().getPrimitive<Rectangle>());
						renderer->setMaterial(UIManager::getInstance().getFontMaterial());
						renderer->setDepth(mLayer);
						
						addComponent<Renderer>(renderer);

						mFontRenderers.push_back(renderer);

						newRenderersCreated = true;
					}

					renderer->setPositionOffset(Vector3(((i* mSize.x) - (mSize.x / 2.0f)) / RenderContext::getAspectRatio(), 0, 0));
					renderer->setTextureRegion(Rectangle(textureCoordinates.x, textureCoordinates.y, textureSize.x, textureSize.y));
				}
			}

			mString = text;

			// NOTE: IMPORTANT - REFRESH COMPONENTS
			setComponentsCache();
		}
	}

    CPP void setVisibility(bool visibility) override
	{
		UIElement::setVisibility(visibility);

		if(mBackground)
		{
			mBackground->setVisibility(visibility);
		}
	}


	GET(IsEditable)
	CRGET(String)
	SET(Size)
	SET(Layer)
};