#pragma once

#include "Core/EntityComponent/ComponentPtr.hpp"
#include "SpacePartition/OcTree.hpp"

#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShader.hpp"
#include "GPU/RenderItem/GPURenderItem.hpp"
#include "Graphics/Renderer/TextureAnimation/TextureAnimation.hpp"

class GPUInstanceRenderer;
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

    const GPURenderItemData& getRendererData() const
    {
        return mGPURenderItem->getGPURenderItemData();
    }

    const Matrix4& getRendererModelMatrix() const
    {
        return mGPURenderItem->getRendererModelMatrix();
    }

    WeakPtr<GPUShaderPropertiesInstance> getGPUShaderPropertiesInstance()
    {
        return mGPURenderItem->getGPUShaderPropertiesInstance();
    }
    const Slot& getRenderSlot() const
    {
        return mGPURenderItem->getRenderSlot();
    }
    const Slot& getInstanceSlot() const
    {
        return mGPURenderItem->getInstanceSlot();
    }
    void setRenderSlot(const Slot& slot)
    {
        mGPURenderItem->setRenderSlot(slot);
    }
    void setInstanceSlot(const Slot& slot)
    {
        mGPURenderItem->setInstanceSlot(slot);
    }

private:
    void calculateRendererModelMatrix();
    const TextureAnimation* getCurrentTextureAnimation() const;
    void updateTextureRegion();

private:
    TextureAnimationUpdater mCurrentTextureAnimationUpdater;
    bool mUpdateMatrix = false;

    OwnerPtr<GPURenderItem> mGPURenderItem;

public:
    HashedString mCurrentTextureAnimationKey;

public:
    GET(GPURenderItem)
    GET_SET(UpdateMatrix)
};
REGISTER_CLASS(MeshRenderer);
