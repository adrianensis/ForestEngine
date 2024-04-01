#pragma once

#include "Engine/Systems/System.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/Material/Material.hpp"

class RenderSharedContext : public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    u32 requestSharedBufferBindingPoint(GPUBufferType gpuSharedBufferType);
    void setInstanceMatrix(const Slot& slot, const Matrix4& matrix);
    void setMaterialInstanceProperties(const Slot& slot, const MaterialInstance& materialInstance);
    void initMaterialInstancePropertiesSharedBuffer(const PoolHandler<Material>& material);
    const GPUSharedBuffer& getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const;
    Slot requestMaterialInstanceSlot(const PoolHandler<Material>& material);
    void freeMaterialInstanceSlot(const PoolHandler<Material>& material, Slot& slot);

private:
    class SharedContextMaterialInstancedData
    {
    public:
        ByteBuffer mMaterialPropertiesBlockArray;
        GPUSharedBuffersContainer mGPUSharedBuffersContainer;
        PoolHandler<Material> mMaterial;
        SlotsManager mSlotsManager;
    };

	std::unordered_map<u32, SharedContextMaterialInstancedData> mMaterialInstancesDataMap;

    GPUSharedBuffersContainer mGPUSharedBuffersContainer;
	std::vector<Matrix4> mMatrices;
    SlotsManager mRenderInstancesSlotsManager;

    u32 mBindingPointsIndexUniform = 0;
    u32 mBindingPointsIndexStorage = 0;
    i32 mMaxSharedBufferBindingPointsUniform = 0;
    i32 mMaxSharedBufferBindingPointsStorage = 0;

    u32 mMaxInstances = 5000;

public:
    RGET(GPUSharedBuffersContainer)
    CRGET(GPUSharedBuffersContainer)
    RGET(RenderInstancesSlotsManager)
};
REGISTER_CLASS(RenderSharedContext)
