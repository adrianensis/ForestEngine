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

void UIText::onDestroy() 
{
	UIElement::onDestroy();
}

void UIText::setText(const std::string& text) 
{
	if (mString != text)
	{
		if (!mFontRenderers.empty())
		{
			if (text.empty())
			{
				mFontRenderers.clear();
			}
			else
			{
				if (mFontRenderers.size() > text.length())
				{
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
					renderer->setUseDepth(true);
					renderer->setDepth(mLayer);
					renderer->setStencilValue(mConfig.mStencilValue);
                    renderer->setStencilFunction(mConfig.mStencilFunction);
					
					addComponent<Renderer>(renderer);

					mFontRenderers.push_back(renderer);
				}

				renderer->setPositionOffset(Vector3(((i* mSize.x) - (mSize.x / 2.0f)) / RenderContext::getAspectRatio(), 0, 0));
				renderer->setTextureRegion(Rectangle(textureCoordinates, textureSize));
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