#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUShader.hpp"

class GPUBuffersLayout;
class Material;
class Shader;

class ShaderUtils
{
public:
    static OwnerPtr<GPUShader> createShader(const GPUBuffersLayout& gpuBuffersLayout, Ptr<const Material> material);
};