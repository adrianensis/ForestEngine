#include "UI/UIElements/UIPanel.hpp"

#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIManager.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void UIArea::initFromConfig(const UIElementConfig& config) 
{
    UIElement::initFromConfig(config);

    if(mConfig.mParent)
    {
        mConfig.mParent->mTransform->addChild(mTransform);
    }

    mTransform->setLocalPosition(mConfig.mDisplayPosition);
    mTransform->setLocalScale(Vector3(mConfig.mDisplaySize, 1));
    mTransform->mIgnoreParentScale = true;
}

UIElementConfig UIArea::calculateConfig(const UIElementConfig& config)
{
    UIElementConfig newConfig = config;
    newConfig.mDisplayPosition = config.mPosition;
    newConfig.mDisplaySize = config.mSize;

	if (newConfig.mAdjustSizeToText)
	{
        Vector2 textSize(0,0);
        Core::f32 maxAscender = 0;
        Core::f32 maxDescender = 0;
        FOR_ARRAY(i, newConfig.mText.get())
        {
            char character = newConfig.mText.get().at(i);
            const Font::FontGlyphData& glyphData = GET_SYSTEM(UIManager).getGlyphData(character);

            textSize.x += glyphData.mAdvance.x;
            maxAscender = std::max(glyphData.mMetrics.mHoriBearing.y, maxAscender);
            maxDescender = std::max(glyphData.mMetrics.mSize.y - glyphData.mMetrics.mHoriBearing.y, maxDescender);
        }
        textSize.y = maxAscender + maxDescender;

		newConfig.mSize = textSize * newConfig.mTextScale;
		newConfig.mDisplaySize = UIUtils::toScreenSpace(newConfig.mSize);
	}

    // translate to top left corner
	newConfig.mDisplayPosition.x += newConfig.mDisplaySize.x / 2.0f;
    newConfig.mDisplayPosition.y -= newConfig.mDisplaySize.y / 2.0f;

    return newConfig;
}

void UIPanel::initFromConfig(const UIElementConfig& config) 
{
    UIArea::initFromConfig(config);

    GPURenderItemData rendererData;
    rendererData.mMesh = GET_SYSTEM(GPUMeshFactory).getPrimitive<Rectangle>();
    rendererData.mShader = mConfig.mShader;
    rendererData.mGPUShaderStencilData = calculateStencilData();
    rendererData.mRenderPassIDs = {
        Core::ClassManager::getClassMetadata<RenderPassUI>().mClassDefinition.getId(),
    };

	EC::TComponentPtr<MeshRenderer> renderer = ECManager.requestComponent<MeshRenderer>();
	renderer->init(rendererData);
    ECManager.addComponent(EC::TEntityPtr(this), renderer);

	renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mDepth = mConfig.mLayer;
	renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mColor = mConfig.mStyle->mBackgroundColor;
    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();
}