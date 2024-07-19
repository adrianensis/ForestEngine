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

    TypedComponentHandler<MeshRenderer> renderer = createComponent<MeshRenderer>(rendererData);
    renderer->getMaterialInstance()->mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mDepth = mConfig.mLayer;
    Rectangle textureRegion = GET_SYSTEM(UIManager).getGlyphData(mCharacter).mTextureRegion;
    renderer->getMaterialInstance()->mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mTextureRegionLeftTop = textureRegion.getLeftTopFront();
    renderer->getMaterialInstance()->mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mTextureRegionSize = textureRegion.getSize();
    renderer->getMaterialInstance()->setDirty();
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
    PROFILER_CPU()
	if (mString != text)
	{
        const u32 textLen = text.get().length();
        const u32 glyphRenderersLen = static_cast<u32>(mFontRenderers.size());
        if(textLen < glyphRenderersLen)
        {
            u32 diff = glyphRenderersLen - textLen;
            FOR_RANGE(i, 0, diff)
            {
                mScene->removeGameObject(mFontRenderers.back());
                mFontRenderers.pop_back();
            }
        }

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
            Vector2 maxDescenderVecScreenSpace(UIUtils::toScreenSpace(maxDescenderVec * mConfig.mTextScale));
            f32 baseLineScreenSpace = mConfig.mDisplaySize.y - maxDescenderVecScreenSpace.y;
            
            f32 offset = -mConfig.mDisplaySize.x/2.0f;
			FOR_RANGE(i, 0, textLen)
			{
                char character = text.get().at(i);
                const FontGlyphData& glyphData = GET_SYSTEM(UIManager).getGlyphData(character);
                Vector2 glyphSize = glyphData.mMetrics.mSize * mConfig.mTextScale;
                Vector2 glyphSizeScreenSpace = UIUtils::toScreenSpace(glyphSize);

                Vector2 bearing(glyphData.mMetrics.mHoriBearing.x, glyphData.mMetrics.mHoriBearing.y);
                Vector2 bearingScreenSpace = UIUtils::toScreenSpace(bearing * mConfig.mTextScale);
                Vector2 glyphPositionScreenSpace(offset + bearingScreenSpace.x, mConfig.mDisplaySize.y/2.0f - baseLineScreenSpace + bearingScreenSpace.y);

                if(i < mFontRenderers.size())
                {
                    TypedEntityHandler<UITextGlyph> gameObjectGlyph = mFontRenderers[i];
                    UIElementConfig glyphConfig = gameObjectGlyph->getConfig();
                    glyphConfig.mPosition = glyphPositionScreenSpace;
                    glyphConfig.mSize = glyphSizeScreenSpace;
                    glyphConfig = gameObjectGlyph->calculateConfig(glyphConfig);
                    gameObjectGlyph->mTransform->setLocalPosition(glyphConfig.mDisplayPosition);
                    gameObjectGlyph->mTransform->setLocalScale(Vector3(glyphConfig.mDisplaySize, 1));
                    TypedComponentHandler<MeshRenderer> renderer = gameObjectGlyph->getFirstComponent<MeshRenderer>();
                    Rectangle textureRegion = GET_SYSTEM(UIManager).getGlyphData(character).mTextureRegion;
                    renderer->getMaterialInstance()->mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mTextureRegionLeftTop = textureRegion.getLeftTopFront();
                    renderer->getMaterialInstance()->mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mTextureRegionSize = textureRegion.getSize();
                    renderer->getMaterialInstance()->setDirty();
                }
                else
                {
                    UIBuilder uiBuilder;
                    TypedEntityHandler<UITextGlyph> gameObjectGlyph = uiBuilder.
                    setPosition(glyphPositionScreenSpace).
                    setIsStatic(mConfig.mIsStaticText).
                    setSize(glyphSizeScreenSpace).
                    setText(HashedString(std::string() + character)).
                    setLayer(mConfig.mLayer + 1).
                    setIsAffectedByLayout(false).
                    setParent(EntityHandler::getEntityHandler(*this)).
                    create<UITextGlyph>().
                    getUIElement<UITextGlyph>();

                    mFontRenderers.push_back(gameObjectGlyph);
                }

                offset += UIUtils::toScreenSpace(Vector2(glyphData.mAdvance.x * mConfig.mTextScale,0)).x;
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