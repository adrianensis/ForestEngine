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
        mTransform.get().mParent = mConfig.mParent.get().mTransform;
    }

    mTransform.get().mLocalPosition = mConfig.mDisplayPosition;
    mTransform.get().mScale = Vector3(UIUtils::correctAspectRatio_X(mConfig.mSize));
    mTransform.get().mAffectedByProjection = false;

    OwnerPtr<Renderer> renderer = OwnerPtr<Renderer>::newObject();
    renderer.get().init();

    renderer.get().mMesh = GET_SYSTEM(MeshPrimitives).getPrimitive<Rectangle>();
    renderer.get().mMaterial = (mConfig.mMaterial);
    renderer.get().mColor = (mConfig.mStyle->mBackgroundColor);
    renderer.get().mUseDepth = (true);
    renderer.get().mDepth = (mConfig.mLayer);
    renderer.get().mStencilValue = (mConfig.mStencilValue);
    renderer.get().mStencilFunction = (mConfig.mStencilFunction);

    addComponent<Renderer>(renderer);
    
    setComponentsCache();
}
