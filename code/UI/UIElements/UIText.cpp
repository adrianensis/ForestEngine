#include "UI/UIElements/UIText.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIElements/UIPanel.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Transform.hpp"

void UITextGlyph::initFromConfig(const UIElementConfig& config) 
{
	UIArea::initFromConfig(config);

    CHECK_MSG(mConfig.mText.size() == 1, "UITextGlyph mConfig.mText must be 1 character only");

    mCharacter = mConfig.mText.at(0);

    RendererData rendererData;
    rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
    rendererData.mMaterial = Ptr<const Material>::cast(GET_SYSTEM(UIManager).getFontMaterial());
    rendererData.mStencilData = calculateStencilData();

    Ptr<MeshRenderer> renderer = createComponent<MeshRenderer>(rendererData);
    renderer->mUseDepth = (true);
    renderer->setDepth(mConfig.mLayer);
    Rectangle textureRegion = GET_SYSTEM(UIManager).getGlyphData(mCharacter).mTextureRegion;
    renderer->setTextureRegion(textureRegion);

	setComponentsCache();
}

void UIText::initFromConfig(const UIElementConfig& config) 
{
	UIArea::initFromConfig(config);

	setLayer(mConfig.mLayer);

	setBackground(mConfig);

	setText(mConfig.mText);

	setComponentsCache();
}

void UIText::onDestroy() 
{
	UIArea::onDestroy();

    mScene->removeGameObject(Ptr<GameObject>::cast(mBackground));

    FOR_LIST(it, mFontRenderers)
    {
        mScene->removeGameObject(Ptr<GameObject>::cast(*it));
    }
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
            f32 offset = -mConfig.mDisplaySize.x/2.0f;
			FOR_RANGE(i, 0, text.length())
			{
                char character = text.at(i);
                Vector2 glyphSize = GET_SYSTEM(UIManager).getGlyphData(character).mMetrics.mSize;
                Vector2 glyphHorizontalBearing = glyphSize - GET_SYSTEM(UIManager).getGlyphData(character).mMetrics.mHoriBearing;
                Vector2 topLeftCorner = GET_SYSTEM(UIManager).getGlyphData(character).mBitmapTopLeft;
                Vector2 topLeftCornerScreenSpace = UIUtils::toScreenSpace(topLeftCorner);
                Vector2 glyphSizeScreenSpace = UIUtils::toScreenSpace(glyphSize);
                Vector2 glyphHorizontalBearingScreenSpace = UIUtils::toScreenSpace(glyphHorizontalBearing);

                UIBuilder uiBuilder;

                Ptr<UITextGlyph> gameObjectGlyph = uiBuilder.
                setPosition(Vector2(offset,topLeftCornerScreenSpace.y -mConfig.mDisplaySize.y/2.0f)).
                setSize(glyphSizeScreenSpace).
                setText(std::string() + character).
                setLayer(mConfig.mLayer + 1).
                setIsStatic(false).
                setIsAffectedByLayout(false).
                setParent(Ptr<GameObject>::cast(getPtrToThis())).
                create<UITextGlyph>().
                getUIElement<UITextGlyph>();

                mFontRenderers.push_back(gameObjectGlyph);

                offset += glyphSizeScreenSpace.x;
			}
		}

		mString = text;

		setComponentsCache();
	}
}

void UIText::setVisibility(bool visibility) 
{
	UIArea::setVisibility(visibility);

    FOR_LIST(it, mFontRenderers)
    {
        (*it)->setVisibility(visibility);
    }

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