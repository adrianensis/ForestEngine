#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Light/Light.hpp"

void GPUSharedContext::init()
{
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &mMaxSharedBufferBindingPointsUniform);
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &mMaxSharedBufferBindingPointsStorage);
    u32 bindingPoint = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mGlobalMatrices.mType);
    mGlobalMatricesBlock.init(bindingPoint, GPUBuiltIn::SharedBuffers::mGlobalMatrices, false);
    bindingPoint = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mLights.mType);
    mLightsBlock.init(bindingPoint, GPUBuiltIn::SharedBuffers::mLights, false);
    GET_SYSTEM(GPUSharedContext).mLightsBlock.resize(10);

    std::vector<LightData> ligths;
    ligths.resize(10);
    ligths[0].mPosition.x = 0;
    mLightsBlock.setDataArray(ligths);
}

u32 GPUSharedContext::requestSharedBufferBindingPoint(GPUSharedBufferType gpuSharedBufferType)
{
    u32 bindingPoint = 0;
    switch (gpuSharedBufferType)
    {
    case UNIFORM:
        bindingPoint = mBindingPointsIndexUniform;
        mBindingPointsIndexUniform++;
        CHECK_MSG((i32)mBindingPointsIndexUniform <= mMaxSharedBufferBindingPointsUniform, "Max Uniform Binding Points reached!");
        break;
    case STORAGE:
        bindingPoint = mBindingPointsIndexStorage;
        mBindingPointsIndexStorage++;
        CHECK_MSG((i32)mBindingPointsIndexStorage <= mMaxSharedBufferBindingPointsStorage, "Max Storage Binding Points reached!");
        break;
    }

    return bindingPoint;
}

void GPUSharedContext::terminate()
{
    mGlobalMatricesBlock.terminate();
    mLightsBlock.terminate();
}