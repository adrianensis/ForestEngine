#include "UI/UIElements/UIText.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIElements/UIPanel.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Transform.hpp"


UIText::UIText()
{
	mIsEditable = false;
}

void UIText::init() 
{
	UIElement::init();
}

void UIText::initFromConfig(const UIElementConfig& config) 
{
	UIElement::initFromConfig(config);

	Vector3 textSize = Vector3(UIUtils::correctAspectRatio_X(mConfig.mTextSize), 1);

	mTransform->mLocalPosition = mConfig.mDisplayPosition;
	mTransform->mScale = textSize;
	mTransform->mAffectedByProjection = false;

	if (mConfig.mParent)
	{
		mTransform->mParent = mConfig.mParent->mTransform;
	}

	setLayer(mConfig.mLayer);

	setBackground(mConfig);

	setText(mConfig.mText);

	setComponentsCache();
}

void UIText::onDestroy() 
{
	UIElement::onDestroy();
}

void UIText::setText(const std::string& text) 
{
	if (mString != text)
	{
		FOR_LIST(it, mFontRenderers)
        {
            removeComponent(*it);
        }
        mFontRenderers.clear();

		if (!text.empty())
		{
			FOR_RANGE(i, 0, text.length())
			{
				Ptr<SpriteRenderer> renderer;

				char character = text.at(i);
				Vector2 textureCoordinates = GET_SYSTEM(UIManager).getCharTextureCoordinates(character);
				Vector2 textureSize = GET_SYSTEM(UIManager).getFontTileTextureSize();

                RendererData rendererData;
                rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
                rendererData.mMaterial = (GET_SYSTEM(UIManager).getFontMaterial());
                rendererData.mStencilData = calculateStencilData();
				rendererData.mPositionOffset = (Vector3(((i* mConfig.mTextSize.x + (mConfig.mTextSize.x/2.0f)) - (mConfig.mTextSize.x * mConfig.mText.length() / 2.0f)) / GET_SYSTEM(RenderContext).getAspectRatio(), 0, 0));
                
                renderer = createComponent<SpriteRenderer>(rendererData);
                renderer->mUseDepth = (true);
	            renderer->setDepth(mLayer);
				renderer->setTextureRegion(Rectangle(textureCoordinates, textureSize));

                mFontRenderers.push_back(renderer);
			}
		}

		mString = text;

		setComponentsCache();
	}
}

void UIText::setVisibility(bool visibility) 
{
	UIElement::setVisibility(visibility);

	if(mBackground)
	{
		mBackground->setVisibility(visibility);
	}
}

void UIText::setIsEditable(bool editable)
{
	if(editable and !getIsEditable())
	{
		subscribeToMouseEvents();
		subscribeToEnterEvent();
		subscribeToEscEvent();
		subscribeToCharEvents();
		mOnlyReleaseOnClickOutside = true;
	}

	if(!editable and getIsEditable())
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
