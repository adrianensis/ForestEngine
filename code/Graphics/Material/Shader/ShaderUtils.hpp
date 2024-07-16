#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUProgram.hpp"

class GPUVertexBuffersContainer;
class GPUSharedBuffersContainer;
class Material;
class Shader;
class Shader;

class ShaderUtils
{
public:
    static OwnerPtr<GPUProgram> compileShader(HashedString label, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const Material& material, Ptr<const Shader> customShader);
};