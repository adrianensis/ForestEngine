#include "UI/UIElements/UIText.hpp"
#include "Core/EntityComponent/Entity.hpp"

#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Window/Window.hpp"

#include "UI/UIManager.hpp"
#include "UI/UIElements/UIPanel.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Transform.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "Graphics/Mesh/MeshFactory.hpp"

void UITextGlyph::initFromConfig(const UIElementConfig& config) 
{
	UIArea::initFromConfig(config);

    CHECK_MSG(mConfig.mText.get().size() == 1, "UITextGlyph mConfig.mText must be 1 character only");

    mCharacter = mConfig.mText.get().at(0);

    GPURenderItemData rendererData;
    rendererData.mMesh = MeshFactory::getInstance().getPrimitive<Maths::Rectangle>();
    rendererData.mShader = GET_SYSTEM(UIManager).getFontShader();
    rendererData.mGPUShaderStencilData = calculateStencilData();
    rendererData.mRenderPassIDs = {
        Core::ClassManager::getClassMetadata<RenderPassUI>().mClassDefinition.getId()
    };

	MeshRenderer* renderer = ECManager.requestComponent<MeshRenderer>();
	renderer->init(rendererData);
    ECManager.addComponent(this, renderer);

    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mDepth = mConfig.mLayer;
    Maths::Cube textureRegion = GET_SYSTEM(UIManager).getGlyphData(mCharacter).mTextureRegion;
    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mTextureRegionLeftTop = textureRegion.getLeftTopFront();
    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mTextureRegionSize = textureRegion.getSize();
    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();
}

void UIText::initFromConfig(const UIElementConfig& config) 
{
	UIArea::initFromConfig(config);
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

void UIText::setText(Core::HashedString text) 
{
    PROFILER_CPU()
	if (mString != text)
	{
        const Core::u32 textLen = text.get().length();
        const Core::u32 glyphRenderersLen = static_cast<Core::u32>(mFontRenderers.size());
        if(textLen < glyphRenderersLen)
        {
            Core::u32 diff = glyphRenderersLen - textLen;
            FOR_RANGE(i, 0, diff)
            {
                mScene->removeGameObject(mFontRenderers.back());
                mFontRenderers.pop_back();
            }
        }

		if (!text.get().empty())
		{
            Core::u32 fontMaxDescender = GET_SYSTEM(UIManager).getFont()->getFontData().mMaxDescender;
            Core::f32 fontMaxDescenderScreenSpace = UIUtils::toScreenSpace(Maths::Vector2(0, fontMaxDescender)).y;
            Core::f32 offset = -mConfig.mDisplaySize.x/2.0f;

			FOR_RANGE(i, 0, textLen)
			{
                char character = text.get().at(i);
                const Font::FontGlyphData& glyphData = GET_SYSTEM(UIManager).getGlyphData(character);
                Maths::Vector2 glyphSize = glyphData.mMetrics.mSize * mConfig.mTextScale;
                Maths::Vector2 glyphSizeScreenSpace = UIUtils::toScreenSpace(glyphSize);

                Maths::Vector2 bearing(glyphData.mMetrics.mHoriBearing.x, glyphData.mMetrics.mHoriBearing.y);
                Maths::Vector2 bearingScreenSpace = UIUtils::toScreenSpace(bearing * mConfig.mTextScale);
                Maths::Vector2 glyphPositionScreenSpace(offset + bearingScreenSpace.x, 0);

                // Move the glyph down half size
                glyphPositionScreenSpace.y = glyphPositionScreenSpace.y - mConfig.mDisplaySize.y/2.0f;
                // Move up bearing size so we align all glyphs bottoms
                glyphPositionScreenSpace.y = glyphPositionScreenSpace.y + bearingScreenSpace.y;
                // Move up equivalent to fontMaxDescenderScreenSpace (so we make room for characters like 'p' or 'g' which cross text baseline) 
                glyphPositionScreenSpace.y = glyphPositionScreenSpace.y + fontMaxDescenderScreenSpace;

                if(i < mFontRenderers.size())
                {
                    UITextGlyph* gameObjectGlyph = mFontRenderers[i];
                    UIElementConfig glyphConfig = gameObjectGlyph->getConfig();
                    glyphConfig.mPosition = glyphPositionScreenSpace;
                    glyphConfig.mSize = glyphSizeScreenSpace;
                    glyphConfig = gameObjectGlyph->calculateConfig(glyphConfig);
                    gameObjectGlyph->mTransform->setLocalPosition(glyphConfig.mDisplayPosition);
                    gameObjectGlyph->mTransform->setLocalScale(Maths::Vector3(glyphConfig.mDisplaySize, 1));
                    MeshRenderer* renderer = ECManager.getFirstComponent<MeshRenderer>(gameObjectGlyph);
                    Maths::Cube textureRegion = GET_SYSTEM(UIManager).getGlyphData(character).mTextureRegion;
                    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mTextureRegionLeftTop = textureRegion.getLeftTopFront();
                    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mTextureRegionSize = textureRegion.getSize();
                    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();
                }
                else
                {
                    UIBuilder uiBuilder;
                    UITextGlyph* gameObjectGlyph = uiBuilder.
                    setPosition(glyphPositionScreenSpace).
                    setIsStatic(mConfig.mIsStaticText).
                    setSize(glyphSizeScreenSpace).
                    setText(Core::HashedString(std::string() + character)).
                    setLayer(mConfig.mLayer + 1).
                    setIsAffectedByLayout(false).
                    setParent(this).
                    create<UITextGlyph>().
                    getUIElement<UITextGlyph>();

                    mFontRenderers.push_back(gameObjectGlyph);
                }

                offset += UIUtils::toScreenSpace(Maths::Vector2(glyphData.mAdvance.x * mConfig.mTextScale,0)).x;
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