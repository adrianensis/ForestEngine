#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUProgram.hpp"

class GPUVertexBuffersContainer;
class GPUSharedBuffersContainer;
class Material;
class Shader;
class MaterialRuntime;

class ShaderUtils
{
public:
    static OwnerPtr<GPUProgram> createShader(ConstString label, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer, const Material& material);
    static OwnerPtr<GPUProgram> createShaderCustomFragment(ConstString label, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer, const Material& material, Ptr<const MaterialRuntime> customMaterialRuntime);
};