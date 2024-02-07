#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPULight.hpp"

void GPUSharedContext::init()
{
    mMaxSharedBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForSharedBuffer(GPUBufferType::UNIFORM);
    mMaxSharedBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForSharedBuffer(GPUBufferType::STORAGE);
    u32 bindingPoint = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mGlobalData.mType);

    u32 bindingPointGlobalData = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mModelMatrices.mType);
    mGPUSharedBuffersContainer.createSharedBuffer(bindingPointGlobalData, GPUBuiltIn::SharedBuffers::mGlobalData, false);
    mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).resize<GPUBuiltIn::SharedBuffers::GPUGlobalData>(1);

    u32 bindingPointLightsData = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mLightsData.mType);
    mGPUSharedBuffersContainer.createSharedBuffer(bindingPointLightsData, GPUBuiltIn::SharedBuffers::mLightsData, false);
    mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mLightsData).resize<GPULightsData>(1);

    mAvailableSlots.resize(5000);
    u32 size = mAvailableSlots.size();
    FOR_RANGE(i, 0, size)
    {
        mAvailableSlots[i] = true;
    }

    mMatrices.resize(size);
    u32 bindingPointModelMatrices = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mModelMatrices.mType);
    mGPUSharedBuffersContainer.createSharedBuffer(bindingPointModelMatrices, GPUBuiltIn::SharedBuffers::mModelMatrices, false);
    mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).resize<Matrix4>(size);

    mMaterialInstancedPropertiesArray.resize(size);
    u32 bindingPointMaterialInstancedProperties = requestSharedBufferBindingPoint(DefaultMaterialInstancedPropertiesGPUData::smDefaultInstancedPropertiesSharedBufferData.mType);
    mGPUSharedBuffersContainer.createSharedBuffer(bindingPointMaterialInstancedProperties, DefaultMaterialInstancedPropertiesGPUData::smDefaultInstancedPropertiesSharedBufferData, false);
    mGPUSharedBuffersContainer.getSharedBuffer(DefaultMaterialInstancedPropertiesGPUData::smDefaultInstancedPropertiesSharedBufferData).resize<MaterialInstancedProperties>(size);
}

u32 GPUSharedContext::requestSharedBufferBindingPoint(GPUBufferType gpuSharedBufferType)
{
    u32 bindingPoint = 0;
    switch (gpuSharedBufferType)
    {
    case GPUBufferType::UNIFORM:
        {
            bindingPoint = mBindingPointsIndexUniform;
            mBindingPointsIndexUniform++;
            CHECK_MSG((i32)mBindingPointsIndexUniform <= mMaxSharedBufferBindingPointsUniform, "Max Uniform Binding Points reached!");
        }
        break;
    case GPUBufferType::STORAGE:
        {
            bindingPoint = mBindingPointsIndexStorage;
            mBindingPointsIndexStorage++;
            CHECK_MSG((i32)mBindingPointsIndexStorage <= mMaxSharedBufferBindingPointsStorage, "Max Storage Binding Points reached!");
        }
        break;
    default:
        CHECK_MSG(false, "Ilegal GPUBufferType!");
        break;
    }

    return bindingPoint;
}

GPUInstanceSlot GPUSharedContext::requestInstanceSlot()
{
    GPUInstanceSlot slot;
    u32 size = mAvailableSlots.size();
    FOR_RANGE(i, 0, size)
    {
        if(mAvailableSlots[i])
        {
            mAvailableSlots[i] = false;
            slot.set(i);
            break;
        }
    }

    return slot;
}

void GPUSharedContext::freeInstanceSlot(GPUInstanceSlot& slot)
{
    mAvailableSlots[slot.getSlot()] = true;
    slot.reset();
}

void GPUSharedContext::setInstanceMatrix(const GPUInstanceSlot& slot, const Matrix4& matrix)
{
    mMatrices.at(slot.getSlot()) = matrix;
}

void GPUSharedContext::setMaterialInstanceProperties(const GPUInstanceSlot& slot, const MaterialInstancedProperties& materialInstanceProperties)
{
    mMaterialInstancedPropertiesArray.at(slot.getSlot()) = materialInstanceProperties;
}

void GPUSharedContext::terminate()
{
    mGPUSharedBuffersContainer.terminate();
}