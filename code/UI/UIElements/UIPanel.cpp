#include "UI/UIElements/UIPanel.hpp"

#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIStyle.hpp"

void UIArea::initFromConfig(const UIElementConfig& config) 
{
    UIElement::initFromConfig(config);

    if(mConfig.mParent)
    {
        mConfig.mParent->mTransform->addChild(mTransform);
    }

    mTransform->setLocalPosition(mConfig.mDisplayPosition);
    mTransform->setScale(Vector3(mConfig.mDisplaySize, 1));
    mTransform->mAffectedByProjection = false;
}

void UIArea::calculateConfig()
{
    mConfig.mDisplayPosition = Vector3(mConfig.mPosition, 1);
    mConfig.mDisplaySize = Vector3(mConfig.mSize, 1);

    // translate to top left corner
	mConfig.mDisplayPosition.x += mConfig.mDisplaySize.x / 2.0f;
    mConfig.mDisplayPosition.y -= mConfig.mDisplaySize.y / 2.0f;
}

void UIPanel::initFromConfig(const UIElementConfig& config) 
{
    UIArea::initFromConfig(config);

    RendererData rendererData;
    rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
    rendererData.mMaterial = (mConfig.mMaterial);
    rendererData.mStencilData = calculateStencilData();

    Ptr<MeshRenderer> renderer = createComponent<MeshRenderer>(rendererData);
    renderer->mUseDepth = (true);
	renderer->setDepth(mConfig.mLayer);
	renderer->setColor(mConfig.mStyle->mBackgroundColor);
    
    setComponentsCache();
}