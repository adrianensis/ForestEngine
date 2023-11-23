#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

void GPUSharedContext::init()
{
    mGlobalMatricesBlock.init(0, GPUBuiltIn::UniformBlocks::mGlobalMatrices, false);
}

void GPUSharedContext::terminate()
{
    mGlobalMatricesBlock.terminate();
}