#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

void GPUSharedContext::init()
{
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &mMaxUniformBufferBindingPoints);
    u32 bindingPoint = requestUniformBufferBindingPoint();
    mGlobalMatricesBlock.init(bindingPoint, GPUBuiltIn::UniformBlocks::mGlobalMatrices, false);
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
}