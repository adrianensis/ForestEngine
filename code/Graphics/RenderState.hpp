#pragma once

#include "Engine/System/System.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Model/Animation/Animation.hpp"

class RenderState : public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    
    void setInstanceMatrix(const Slot& slot, const Matrix4& matrix);
    Slot requestRenderInstanceSlot();
    void freeRenderInstanceSlot(Slot& slot);
    
    void setMaterialInstanceProperties(const Slot& slot, const MaterialInstance& materialInstance);
    void initMaterialInstancePropertiesSharedBuffer(const PoolHandler<Material>& material);
    const GPUSharedBuffer& getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const;
    Slot requestMaterialInstanceSlot(const PoolHandler<Material>& material);
    void freeMaterialInstanceSlot(const PoolHandler<Material>& material, Slot& slot);

    void initSkeletonRenderState(Ptr<const SkeletonState> skeletonState);
    const GPUSharedBuffer& getSkeletonRenderStateGPUSharedBuffer(Ptr<const SkeletonState> skeletonState) const;

private:
    class MaterialRenderState
    {
    public:
        ByteBuffer mMaterialPropertiesBlockArray;
        GPUSharedBuffersContainer mGPUSharedBuffersContainer;
        PoolHandler<Material> mMaterial;
        SlotsManager mSlotsManager;
    };

	std::unordered_map<u32, MaterialRenderState> mMaterialRenderStates;

    class SkeletonRenderState
    {
    public:
        GPUSharedBuffersContainer mGPUSharedBuffersContainer;
    };

	std::unordered_map<Ptr<const SkeletonState>, SkeletonRenderState> mSkeletonRenderStates;

	std::vector<Matrix4> mMatrices;

    SlotsManager mRenderInstancesSlotsManager;
    u32 mMaxInstances = 5000;

public:
    RGET(RenderInstancesSlotsManager)
};
REGISTER_CLASS(RenderState)
