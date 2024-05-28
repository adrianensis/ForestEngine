#include "UI/UIElements/UIText.hpp"
#include "Graphics/Module.hpp"
#include "UI/UIManager.hpp"
#include "UI/UIElements/UIPanel.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Transform.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void UITextGlyph::initFromConfig(const UIElementConfig& config) 
{
	UIArea::initFromConfig(config);

    CHECK_MSG(mConfig.mText.get().size() == 1, "UITextGlyph mConfig.mText must be 1 character only");

    mCharacter = mConfig.mText.get().at(0);

    RendererData rendererData;
    rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
    rendererData.mMaterial = GET_SYSTEM(UIManager).getFontMaterial();
    rendererData.mStencilData = calculateStencilData();
    rendererData.mRenderPassIDs = {
        ClassManager::getClassMetadata<RenderPassUI>().mClassDefinition.getId()
    };

    Ptr<MeshRenderer> renderer = createComponent<MeshRenderer>(rendererData);
    renderer->getMaterialInstance().mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mDepth = mConfig.mLayer;
    Rectangle textureRegion = GET_SYSTEM(UIManager).getGlyphData(mCharacter).mTextureRegion;
    renderer->getMaterialInstance().mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mTextureRegionLeftTop = textureRegion.getLeftTopFront();
    renderer->getMaterialInstance().mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mTextureRegionSize = textureRegion.getSize();
}

void UIText::initFromConfig(const UIElementConfig& config) 
{
	UIArea::initFromConfig(config);

	setLayer(mConfig.mLayer);

	setBackground(mConfig);

	setText(mConfig.mText);
}

void UIText::onDestroy() 
{
	UIArea::onDestroy();

    mScene->removeGameObject(mBackground);

    FOR_LIST(it, mFontRenderers)
    {
        mScene->removeGameObject(*it);
    }
}

void UIText::setText(HashedString text) 
{
	if (mString != text)
	{
		FOR_LIST(it, mFontRenderers)
        {
            mScene->removeGameObject(*it);
        }
        mFontRenderers.clear();

		if (!text.get().empty())
		{
            f32 maxAscender = 0;
            f32 maxDescender = 0;
            FOR_ARRAY(i, text.get())
            {
                char character = text.get().at(i);
                const FontGlyphData& glyphData = GET_SYSTEM(UIManager).getGlyphData(character);

                maxAscender = std::max(glyphData.mMetrics.mHoriBearing.y, maxAscender);
                maxDescender = std::max(glyphData.mMetrics.mSize.y - glyphData.mMetrics.mHoriBearing.y, maxDescender);
            }

            Vector2 maxDescenderVec(0, maxDescender);
            Vector2 maxDescenderVecScreenSpace(UIUtils::toScreenSpace(maxDescenderVec));
            f32 baseLineScreenSpace = mConfig.mDisplaySize.y - maxDescenderVecScreenSpace.y;
            
            f32 offset = -mConfig.mDisplaySize.x/2.0f;
			FOR_RANGE(i, 0, text.get().length())
			{
                char character = text.get().at(i);
                const FontGlyphData& glyphData = GET_SYSTEM(UIManager).getGlyphData(character);
                Vector2 glyphSize = glyphData.mMetrics.mSize;
                Vector2 glyphSizeScreenSpace = UIUtils::toScreenSpace(glyphSize);

                Vector2 bearing(glyphData.mMetrics.mHoriBearing.x, glyphData.mMetrics.mHoriBearing.y);
                Vector2 bearingScreenSpace = UIUtils::toScreenSpace(bearing);
                Vector2 glyphPositionScreenSpace(offset + bearingScreenSpace.x, mConfig.mDisplaySize.y/2.0f - baseLineScreenSpace + bearingScreenSpace.y);

                UIBuilder uiBuilder;
                Ptr<UITextGlyph> gameObjectGlyph = uiBuilder.
                setPosition(glyphPositionScreenSpace).
                setSize(glyphSizeScreenSpace).
                setText(HashedString(std::string() + character)).
                setLayer(mConfig.mLayer + 1).
                setIsAffectedByLayout(false).
                setParent(Ptr<GameObject>::cast(getPtrToThis<UIText>())).
                create<UITextGlyph>().
                getUIElement<UITextGlyph>();

                mFontRenderers.push_back(gameObjectGlyph);

                offset += UIUtils::toScreenSpace(Vector2(glyphData.mAdvance.x,0)).x;
			}
		}

		mString = text;
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