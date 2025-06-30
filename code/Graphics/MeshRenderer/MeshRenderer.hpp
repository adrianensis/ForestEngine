#pragma once

#include "Core/EntityComponent/Component.hpp"
#include "SpacePartition/OcTree.hpp"

#include "GPU/RenderItem/GPURenderItem.hpp"
#include "GPU/Image/TextureAnimation/GPUTextureAnimation.hpp"

class TextureAnimation;

class MeshRenderer: public Component, public IOcTreeElement
{
    
	DECLARE_SERIALIZATION()
	
public:
    ClassId getComponentTypeId() const override;

    void init(const GPURenderItemData& data);
    void onComponentAdded() override;
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
