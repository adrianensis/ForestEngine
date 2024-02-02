#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUProgram.hpp"

class GPUBuffersContainer;
class Material;
class Shader;

class ShaderUtils
{
public:
    static OwnerPtr<GPUProgram> createShader(const GPUBuffersContainer& gpuBuffersContainer, Ptr<const Material> material);
};