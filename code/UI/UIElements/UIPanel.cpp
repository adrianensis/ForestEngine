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
        f32 maxAscender = 0;
        f32 maxDescender = 0;
        FOR_ARRAY(i, newConfig.mText.get())
        {
            char character = newConfig.mText.get().at(i);
            const FontGlyphData& glyphData = GET_SYSTEM(UIManager).getGlyphData(character);

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

    RendererData rendererData;
    rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
    rendererData.mMaterial = mConfig.mMaterial;
    rendererData.mStencilData = calculateStencilData();
    rendererData.mRenderPassIDs = {
        ClassManager::getClassMetadata<RenderPassUI>().mClassDefinition.getId(),
    };

    TypedComponentHandler<MeshRenderer> renderer = createComponent<MeshRenderer>(rendererData);
	renderer->getMaterialInstance()->mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mDepth = mConfig.mLayer;
	renderer->getMaterialInstance()->mMaterialPropertiesBlockBuffer.get<MaterialPropertiesBlockUI>().mColor = mConfig.mStyle->mBackgroundColor;
    renderer->getMaterialInstance()->setDirty();
}