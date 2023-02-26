#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shaders/ShaderBuilderNodes.hpp"

class GPUBuffersLayout;
class Material;

class ShaderBuilder
{
public:
    ShaderBuilder();

    void createShader(const GPUBuffersLayout& gpuBuffersLayout, Ptr<const Material> material);
    std::string getCode() const;

private:
    ShaderBuilderNodes::Program& get() { return mProgram; }
    ShaderBuilderNodes::Program mProgram;
};