#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Light/Light.hpp"

void GPUSharedContext::init()
{
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &mMaxUniformBufferBindingPoints);
    u32 bindingPoint = requestUniformBufferBindingPoint();
    mGlobalMatricesBlock.init(bindingPoint, GPUBuiltIn::UniformBlocks::mGlobalMatrices, false);
    bindingPoint = requestUniformBufferBindingPoint();
    mLightsBlock.init(bindingPoint, GPUBuiltIn::UniformBlocks::mLights, false);
    GET_SYSTEM(GPUSharedContext).mLightsBlock.resize(10);

    std::vector<LightData> ligths;
    ligths.resize(10);
    ligths[0].mPosition.x = 0;
    mLightsBlock.setDataArray(ligths);
}

u32 GPUSharedContext::requestUniformBufferBindingPoint()
{
    u32 bindingPoint = mBindingPointsIndex;
    mBindingPointsIndex++;
    CHECK_MSG((i32)mBindingPointsIndex <= mMaxUniformBufferBindingPoints, "Max Binding Points reached!");
    return bindingPoint;
}

void GPUSharedContext::terminate()
{
    mGlobalMatricesBlock.terminate();
    mLightsBlock.terminate();
}