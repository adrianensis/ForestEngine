#pragma once

#include "Scene/GameComponent.hpp"
#include "SpacePartition/OcTree.hpp"

#include "GPU/RenderItem/GPURenderItem.hpp"
#include "GPU/Texture/TextureAnimation/GPUTextureAnimation.hpp"

class TextureAnimation;

class MeshRenderer: public GameComponent, public IOcTreeElement
{
    
public:
    void init(const GPURenderItemData& data);
    void onECComponentAdded() override;
    void onDestroy() override;
    void update();

    const GPURenderItemData& getGPURenderItemData() const
    {
        return mGPURenderItem->getGPURenderItemData();
    }

private:
    void calculateRendererModelMatrix();
    const TextureAnimation* getCurrentTextureAnimation() const;
    void updateTextureRegion();

private:
    GPUTextureAnimationUpdater mCurrentTextureAnimationUpdater;
    Core::OwnerPtr<GPURenderItem> mGPURenderItem;

public:
    Core::HashedString mCurrentTextureAnimationKey;

public:
    GET(GPURenderItem)
};
REGISTER_CLASS(MeshRenderer);
