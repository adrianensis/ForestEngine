#include "UI/UIElements/UIPanel.hpp"


#include "Core/Maths/Geometry.hpp"
#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Window/Window.hpp"
#include "Graphics/Mesh/MeshFactory.hpp"

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
    mTransform->setLocalScale(Maths::Vector3(mConfig.mDisplaySize, 1));
    mTransform->mIgnoreParentScale = true;
}

UIElementConfig UIArea::calculateConfig(const UIElementConfig& config)
{
    UIElementConfig newConfig = config;
    newConfig.mDisplayPosition = config.mPosition;
    newConfig.mDisplaySize = config.mSize;

	if (newConfig.mAdjustSizeToText)
	{
        Maths::Vector2 textSize(0,0);
        FOR_ARRAY(i, newConfig.mText.get())
        {
            char character = newConfig.mText.get().at(i);
            const Font::FontGlyphData& glyphData = mSystemsDI.getSystem<UIManager>()->getGlyphData(character);
            textSize.x += glyphData.mAdvance.x;
        }
        textSize.y = mSystemsDI.getSystem<UIManager>()->getFont()->getFontData().mMaxAscender + mSystemsDI.getSystem<UIManager>()->getFont()->getFontData().mMaxDescender;
		newConfig.mSize = textSize * newConfig.mTextScale;
		newConfig.mDisplaySize = UIUtils::toScreenSpace(mSystemsDI.getSystem<UIManager>()->getWindow(), newConfig.mSize);
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
    rendererData.mMesh = MeshFactory::getInstance().getPrimitive<Maths::Rectangle>();
    rendererData.mShader = mConfig.mShader;
    rendererData.mGPUDepthStencilData = calculateStencilData();
    rendererData.mRenderPassIDs = {
        Core::ClassManager::getClassMetadata<RenderPassUI>().mClassDefinition.getId(),
    };

	MeshRenderer* renderer = ECManager.requestComponent<MeshRenderer>(this, [&](auto* component)
    {
        component->init(rendererData);
    });

	renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mDepth = mConfig.mLayer;
	renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mColor = mConfig.mStyle->mBackgroundColor;
    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();
}