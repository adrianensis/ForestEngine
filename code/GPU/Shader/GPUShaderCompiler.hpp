#pragma once

#include "Core/Minimal.hpp"

// INFO: from here https://github.com/KhronosGroup/glslang?tab=readme-ov-file#c-functional-interface-new
#include <glslang/Include/glslang_c_interface.h>

// Required for use of glslang_default_resource
#include <glslang/Public/resource_limits_c.h>

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
    static SPIRVBinary compileShaderToSPIRV(glslang_stage_t stage, const char* shaderSource, const char* fileName);
    static void deleteSPIRVBinary(SPIRVBinary& spirvBinary);
};