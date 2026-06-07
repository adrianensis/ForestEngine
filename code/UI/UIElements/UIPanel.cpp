#include "UI/UIElements/UIPanel.hpp"


#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Window/Window.hpp"
#include "Graphics/Mesh/MeshFactory.hpp"

#include "Scene/Transform.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIManager.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void UIArea::initFromConfig(UIManager* uiManager, const UIElementConfig& config) 
{
    UIElement::initFromConfig(uiManager, config);

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
            const Font::FontGlyphData& glyphData = mUIManager->getGlyphData(character);
            textSize.x += glyphData.mAdvance.x;
        }
        textSize.y = mUIManager->getFont()->getFontData().mMaxAscender + mUIManager->getFont()->getFontData().mMaxDescender;
		newConfig.mSize = textSize * newConfig.mTextScale;
		newConfig.mDisplaySize = UIUtils::toScreenSpace(GET_SYSTEM(Window::WindowManager).getMainWindow().getInternalPointer(), newConfig.mSize);
	}

    // translate to top left corner
	newConfig.mDisplayPosition.x += newConfig.mDisplaySize.x / 2.0f;
    newConfig.mDisplayPosition.y -= newConfig.mDisplaySize.y / 2.0f;

    return newConfig;
}

void UIPanel::initFromConfig(UIManager* uiManager, const UIElementConfig& config) 
{
    UIArea::initFromConfig(uiManager, config);

    GPURenderItemData rendererData;
    rendererData.mMesh = MeshFactory::getInstance().getPrimitive<Maths::Rectangle>().getInternalPointer();
    rendererData.mShader = mConfig.mShader;
    rendererData.mGPUDepthStencilData = calculateStencilData();
    rendererData.mRenderPassIDs = {
        GPURenderPass::getID<RenderPassUI>()
    };

	MeshRenderer* renderer = GET_SYSTEM(EC::EntityComponentManager).requestComponent<MeshRenderer>(this, [&](auto* component)
    {
        component->init(rendererData);
    });

	renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mDepth = mConfig.mLayer;
	renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->mGPUShaderPropertiesBlockBuffer.get<GPUShaderPropertiesBlockUI>().mColor = mConfig.mStyle->mBackgroundColor;
    renderer->getGPURenderItem()->getGPUShaderPropertiesInstance()->setDirty();
}