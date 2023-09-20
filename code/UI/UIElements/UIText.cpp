#include "UI/UIElements/UIText.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIElements/UIPanel.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Transform.hpp"

void UITextGlyph::init() 
{
	UIElement::init();
}

void UITextGlyph::initFromConfig(const UIElementConfig& config) 
{
	UIElement::initFromConfig(config);

	mTransform->setLocalPosition(mConfig.mDisplayPosition);
	mTransform->setScale(Vector3(mConfig.mSize, 1));
	mTransform->mAffectedByProjection = false;

	if (mConfig.mParent)
	{
        mConfig.mParent->mTransform->addChild(mTransform);
	}

    RendererData rendererData;
    rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
    rendererData.mMaterial = Ptr<const Material>::cast(GET_SYSTEM(UIManager).getFontMaterial());
    rendererData.mStencilData = calculateStencilData();
    // rendererData.mPositionOffset = Vector3(offset + (glyphSizeScreenSpace.x/2.0f), 0,1);
    // rendererData.mScaleOffset = Vector3(glyphSizeScreenSpace,1);

    // offset += glyphSizeScreenSpace.x;

    Ptr<MeshRenderer> renderer = createComponent<MeshRenderer>(rendererData);
    renderer->mUseDepth = (true);
    renderer->setDepth(mConfig.mLayer);
    Rectangle textureRegion = GET_SYSTEM(UIManager).getGlyphTextureRegion('A');
    renderer->setTextureRegion(textureRegion);

	setComponentsCache();
}

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

	// mTransform->setLocalPosition(mConfig.mDisplayPosition);
	mTransform->mAffectedByProjection = false;

	if (mConfig.mParent)
	{
        mConfig.mParent->mTransform->addChild(mTransform);
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
            mScene->removeGameObject(Ptr<GameObject>::cast(*it));
        }
        mFontRenderers.clear();

		if (!text.empty())
		{
            f32 offset = 0;
			FOR_RANGE(i, 0, text.length())
			{
                char character = text.at(i);
                Vector2 glyphSize = GET_SYSTEM(UIManager).getGlyphSize(mConfig.mText.at(i));
                Vector2 glyphSizeScreenSpace = UIUtils::toScreenSpace(glyphSize);

                UIBuilder uiBuilder;

                Ptr<UITextGlyph> gameObjectGlyph = uiBuilder.
                setPosition(Vector2(offset + (glyphSizeScreenSpace.x/2.0f),0)).
                setSize(glyphSizeScreenSpace).
                // setTextSize(mConfig.mTextSize).
                setLayer(mConfig.mLayer).
                setIsStatic(false).
                setIsAffectedByLayout(false).
                setParent(Ptr<GameObject>::cast(getPtrToThis())).
                create<UITextGlyph>().
                getUIElement<UITextGlyph>();

                mFontRenderers.push_back(gameObjectGlyph);
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
