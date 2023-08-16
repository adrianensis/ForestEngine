#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shaders/ShaderBuilderNodes.hpp"

class GPUVertexBuffersLayout;
class Material;

class ShaderBuilder
{
public:
    ShaderBuilder();

    void createVertexShader(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material);
    void createFragmentShader(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material);
    std::string getCode() const;

private:

    ShaderBuilderNodes::Program& get() { return mProgram; }
    ShaderBuilderNodes::Program mProgram;
};