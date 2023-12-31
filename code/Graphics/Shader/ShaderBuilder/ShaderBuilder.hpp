#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUVertexBuffersLayout.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"

class GPUVertexBuffersLayout;
class Material;
class Shader;

class ShaderBuilder
{
public:
    ShaderBuilder();

    void createVertexShader(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material);
    void createFragmentShader(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material);
    std::string getCode() const;

private:
    class ShaderBuilderDataCommon
    {
    public:
        std::vector<GPUStructDefinition> mStructDefinitions;
        std::vector<GPUVariableDefinitionData> mUniforms;
        std::vector<GPUSharedBufferData> mSharedBuffers;
        std::vector<GPUVariableDefinitionData> mConsts;
    };

    class ShaderBuilderDataVertex
    {
    public:
        std::vector<GPUVertexBuffer> mVertexInputs;
        std::vector<GPUVariableDefinitionData> mVertexOutputs;
    };

    class ShaderBuilderDataFragment
    {
    public:
        std::vector<GPUVariableDefinitionData> mFragmentInputs;
        std::vector<GPUVariableDefinitionData> mFragmentOutputs;
    };

    class ShaderBuilderData
    {
    public:
        ShaderBuilderDataCommon mCommonVariables;
        ShaderBuilderDataVertex mVertexVariables;
        ShaderBuilderDataFragment mFragmentVariables;
    };

    ShaderBuilderData generateShaderBuilderData(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material) const;
    void registerVertexShaderData(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material);
    void registerFragmentShaderData(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material);
    ShaderBuilderNodes::Program& get() { return mProgram; }

private:
    ShaderBuilderNodes::Program mProgram;
};