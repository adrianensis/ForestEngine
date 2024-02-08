#pragma once

#include "Core/Systems/System.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/Material/Material.hpp"

class GPUInstanceSlot
{
public:
    void set(u32 slot)
    {
        mSlot = slot;
        mIsValid = true;
    }
    void reset()
    {
        mSlot = 0;
        mIsValid = false;
    }
private:
    u32 mSlot = 0;
    bool mIsValid = false;
public:
    GET(Slot)
    GET(IsValid)
};

class GPUSharedContext : public System
{
public:
	GPUSharedContext() = default;

    void init();
    void terminate();
    u32 requestSharedBufferBindingPoint(GPUBufferType gpuSharedBufferType);
    GPUInstanceSlot requestInstanceSlot();
    void freeInstanceSlot(GPUInstanceSlot& slot);
    void setInstanceMatrix(const GPUInstanceSlot& slot, const Matrix4& matrix);
    void setMaterialInstanceProperties(const GPUInstanceSlot& slot, const MaterialInstancedProperties& materialInstanceProperties);

private:
    GPUSharedBuffersContainer mGPUSharedBuffersContainer;
	std::vector<Matrix4> mMatrices;
	std::vector<MaterialInstancedProperties> mMaterialInstancedPropertiesArray;
    std::vector<u32> mAvailableSlots;

private:
    u32 mBindingPointsIndexUniform = 0;
    u32 mBindingPointsIndexStorage = 0;
    i32 mMaxSharedBufferBindingPointsUniform = 0;
    i32 mMaxSharedBufferBindingPointsStorage = 0;

public:
    RGET(GPUSharedBuffersContainer)
    CRGET(GPUSharedBuffersContainer)
    CRGET(Matrices)
    CRGET(MaterialInstancedPropertiesArray)
};