#include "UI/UIElements/UIPanel.hpp"

#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIStyle.hpp"

void UIPanel::initFromConfig(const UIElementConfig& config) 
{
    UIElement::initFromConfig(config);

    if(mConfig.mParent)
    {
        mConfig.mParent->mTransform->addChild(mTransform);
    }

    mTransform->setLocalPosition(mConfig.mDisplayPosition);
    mTransform->setScale(Vector3(mConfig.mDisplaySize, 1));
    mTransform->mAffectedByProjection = false;

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
