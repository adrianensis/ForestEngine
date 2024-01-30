#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUProgram.hpp"

class GPUBuffersLayout;
class Material;
class Shader;

class ShaderUtils
{
public:
    static OwnerPtr<GPUProgram> createShader(const GPUBuffersLayout& gpuBuffersLayout, Ptr<const Material> material);
};