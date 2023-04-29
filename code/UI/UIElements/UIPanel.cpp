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

    OwnerPtr<Renderer> renderer = OwnerPtr<Renderer>::newObject();
    renderer->init();

    renderer->mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
    renderer->mMaterial = (mConfig.mMaterial);
    renderer->mColor = (mConfig.mStyle->mBackgroundColor);
    renderer->mUseDepth = (true);
    renderer->mDepth = (mConfig.mLayer);
    renderer->mStencilValue = (mConfig.mStencilValue);
    renderer->mStencilFunction = (mConfig.mStencilFunction);

    addComponent<Renderer>(std::move(renderer));
    
    setComponentsCache();
}
