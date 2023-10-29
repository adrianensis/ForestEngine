#pragma once

#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

void GPUSharedContext::init()
{
    mMatricesBlock.init(0, GPUBuiltIn::UniformBlocks::mMatrices, false);
}

void GPUSharedContext::terminate()
{
    mMatricesBlock.terminate();
}