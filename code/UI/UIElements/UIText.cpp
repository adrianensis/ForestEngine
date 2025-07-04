#include "UI/UIElements/UIText.hpp"
#include "Engine/EntityComponent/Entity.hpp"

#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Engine/Window/Window.hpp"
#include "GPU/Mesh/GPUMeshFactory.hpp"

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

    GPURenderItemData rendererData;
    rendererData.mMesh = GET_SYSTEM(GPUMeshFactory).getPrimitive<Maths::Cube>();
    rendererData.mShader = GET_SYSTEM(UIManager).getFontShader();
    rendererData.mGPUShaderStencilData = calculateStencilData();
    rendererData.mRenderPassIDs = {
        Core::ClassManager::getClassMetadata<RenderPassUI>().mClassDefinition.getId()
    };

	EC::TComponentPtr<MeshRenderer> renderer = ECManager.requestComponent<MeshRenderer>();
	renderer->init(rendererData);
    ECManager.addComponent(EC::TEntityPtr(this), renderer);

    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mDepth = mConfig.mLayer;
    Maths::Cube textureRegion = GET_SYSTEM(UIManager).getGlyphData(mCharacter).mTextureRegion;
    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mTextureRegionLeftTop = textureRegion.getLeftTopFront();
    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mTextureRegionSize = textureRegion.getSize();
    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();
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

    mScene->removeSceneObject(mBackground);

    FOR_LIST(it, mFontRenderers)
    {
        mScene->removeSceneObject(*it);
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
                mScene->removeSceneObject(mFontRenderers.back());
                mFontRenderers.pop_back();
            }
        }

		if (!text.get().empty())
		{
            Core::f32 maxAscender = 0;
            Core::f32 maxDescender = 0;
            FOR_ARRAY(i, text.get())
            {
                char character = text.get().at(i);
                const Font::FontGlyphData& glyphData = GET_SYSTEM(UIManager).getGlyphData(character);

                maxAscender = std::max(glyphData.mMetrics.mHoriBearing.y, maxAscender);
                maxDescender = std::max(glyphData.mMetrics.mSize.y - glyphData.mMetrics.mHoriBearing.y, maxDescender);
            }

            Maths::Vector2 maxDescenderVec(0, maxDescender);
            Maths::Vector2 maxDescenderVecScreenSpace(UIUtils::toScreenSpace(maxDescenderVec * mConfig.mTextScale));
            Core::f32 baseLineScreenSpace = mConfig.mDisplaySize.y - maxDescenderVecScreenSpace.y;
            
            Core::f32 offset = -mConfig.mDisplaySize.x/2.0f;
			FOR_RANGE(i, 0, textLen)
			{
                char character = text.get().at(i);
                const Font::FontGlyphData& glyphData = GET_SYSTEM(UIManager).getGlyphData(character);
                Maths::Vector2 glyphSize = glyphData.mMetrics.mSize * mConfig.mTextScale;
                Maths::Vector2 glyphSizeScreenSpace = UIUtils::toScreenSpace(glyphSize);

                Maths::Vector2 bearing(glyphData.mMetrics.mHoriBearing.x, glyphData.mMetrics.mHoriBearing.y);
                Maths::Vector2 bearingScreenSpace = UIUtils::toScreenSpace(bearing * mConfig.mTextScale);
                Maths::Vector2 glyphPositionScreenSpace(offset + bearingScreenSpace.x, mConfig.mDisplaySize.y/2.0f - baseLineScreenSpace + bearingScreenSpace.y);

                if(i < mFontRenderers.size())
                {
                    EC::TEntityPtr<UITextGlyph> sceneObjectGlyph = mFontRenderers[i];
                    UIElementConfig glyphConfig = sceneObjectGlyph->getConfig();
                    glyphConfig.mPosition = glyphPositionScreenSpace;
                    glyphConfig.mSize = glyphSizeScreenSpace;
                    glyphConfig = sceneObjectGlyph->calculateConfig(glyphConfig);
                    sceneObjectGlyph->mTransform->setLocalPosition(glyphConfig.mDisplayPosition);
                    sceneObjectGlyph->mTransform->setLocalScale(Maths::Vector3(glyphConfig.mDisplaySize, 1));
                    EC::TComponentPtr<MeshRenderer> renderer = ECManager.getFirstComponent<MeshRenderer>(sceneObjectGlyph);
                    Maths::Cube textureRegion = GET_SYSTEM(UIManager).getGlyphData(character).mTextureRegion;
                    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mTextureRegionLeftTop = textureRegion.getLeftTopFront();
                    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mTextureRegionSize = textureRegion.getSize();
                    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();
                }
                else
                {
                    UIBuilder uiBuilder;
                    EC::TEntityPtr<UITextGlyph> sceneObjectGlyph = uiBuilder.
                    setPosition(glyphPositionScreenSpace).
                    setIsStatic(mConfig.mIsStaticText).
                    setSize(glyphSizeScreenSpace).
                    setText(Core::HashedString(std::string() + character)).
                    setLayer(mConfig.mLayer + 1).
                    setIsAffectedByLayout(false).
                    setParent(this).
                    create<UITextGlyph>().
                    getUIElement<UITextGlyph>();

                    mFontRenderers.push_back(sceneObjectGlyph);
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
		UNSUBSCRIBE_TO_EVENT(Input::InputEventMouseButtonPressed, nullptr, this);
		UNSUBSCRIBE_TO_EVENT(Input::InputEventMouseButtonReleased, nullptr, this);
		UNSUBSCRIBE_TO_EVENT(Input::InputEventChar, nullptr, this);
		UNSUBSCRIBE_TO_EVENT(Input::InputEventKeyBackspace, nullptr, this);
		UNSUBSCRIBE_TO_EVENT(Input::InputEventKeyEnter, nullptr, this);
		UNSUBSCRIBE_TO_EVENT(Input::InputEventKeyEsc, nullptr, this);
		mOnlyReleaseOnClickOutside = true;
	}
}