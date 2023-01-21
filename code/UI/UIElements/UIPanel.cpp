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
    size.x = size.x / RenderContext::getAspectRatio();

    if(mConfig.mParent)
    {
        mTransform.get().mParent = &mConfig.mParent->mTransform.get();
    }

    mTransform.get().mLocalPosition = mConfig.mDisplayPosition;
    mTransform.get().mScale = Vector3(UIUtils::correctAspectRatio_X(mConfig.mSize));
    mTransform.get().mAffectedByProjection = false;

    Renderer *renderer = NEW(Renderer);
    renderer->init();

    renderer->mMesh = MeshPrimitives::getInstance().getPrimitive<Rectangle>();
    renderer->mMaterial = (mConfig.mMaterial);
    renderer->mColor = (mConfig.mStyle->mBackgroundColor);
    renderer->mUseDepth = (true);
    renderer->mDepth = (mConfig.mLayer);
    renderer->mStencilValue = (mConfig.mStencilValue);
    renderer->mStencilFunction = (mConfig.mStencilFunction);

    addComponent<Renderer>(renderer);
    
    setComponentsCache();
}
