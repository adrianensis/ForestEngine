#pragma once

#include "GPU/Core/GPUDefinitions.h"

class SPIRVBinary
{
public:
    // SPIR-V words
    u32* mWords = nullptr;
    // number of words in SPIR-V binary
    u32 mSize = 0;
};

class GPUShaderCompiler
{
public:
    // SPIR-V words
    static SPIRVBinary compileShaderToSPIRV(GPUPipelineStage stage, const char* shaderSource, const char* fileName);
    static void deleteSPIRVBinary(SPIRVBinary& spirvBinary);
};