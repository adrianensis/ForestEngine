#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUShader.hpp"

class GPUVertexBuffersLayout;
class Material;
class Shader;

class ShaderUtils
{
public:
    static OwnerPtr<GPUShader> createShader(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material);
};