#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPULight.hpp"

void GPUSharedContext::init()
{
    mMaxSharedBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForSharedBuffer(GPUBufferType::UNIFORM);
    mMaxSharedBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForSharedBuffer(GPUBufferType::STORAGE);
    u32 bindingPoint = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mGlobalMatrices.mType);
    mGlobalMatricesBuffer.init(bindingPoint, GPUBuiltIn::SharedBuffers::mGlobalMatrices, false);
    mGlobalMatricesBuffer.resize(1);
    bindingPoint = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mLights.mType);
    mLightsBuffer.init(bindingPoint, GPUBuiltIn::SharedBuffers::mLights, false);
    mLightsBuffer.resize(10);
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

void GPUSharedContext::terminate()
{
    mGlobalMatricesBuffer.terminate();
    mLightsBuffer.terminate();
}