#include "UI/UIElements/UIPanel.hpp"

#include "Graphics/Module.hpp"
#include "Scene/Transform.hpp"
#include "UI/UIElementConfig.hpp"
#include "UI/UIStyle.hpp"

void UIPanel::initFromConfig(const UIElementConfig& config) 
{
    UIElement::initFromConfig(config);

    Vector3 size = mConfig.mSize;
    size.z = 1;
    size.x = size.x / GET_SYSTEM(RenderContext).getAspectRatio();

    if(mConfig.mParent)
    {
        mTransform->mParent = mConfig.mParent->mTransform;
    }

    mTransform->mLocalPosition = mConfig.mDisplayPosition;
    mTransform->mScale = Vector3(UIUtils::correctAspectRatio_X(mConfig.mSize));
    mTransform->mAffectedByProjection = false;

    RendererData rendererData;
    rendererData.mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
    rendererData.mMaterial = (mConfig.mMaterial);
    rendererData.mStencilData = calculateStencilData();

    Ptr<SpriteRenderer> renderer = createComponent<SpriteRenderer>(rendererData);
    renderer->mUseDepth = (true);
	renderer->setDepth(mConfig.mLayer);
	renderer->setColor(mConfig.mStyle->mBackgroundColor);
    
    setComponentsCache();
}
