#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"

class Material;

class MaterialRuntimeData
{
public:
    PoolHandler<Material> mMaterial;
    GPUStructDefinition mPropertiesBlockStructDefinition;
    GPUSharedBufferData mPropertiesBlockSharedBufferData;
};

class MaterialRuntime
{
protected:
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
        std::vector<GPUVariableDefinitionData> mUniforms;
        std::vector<GPUVariableDefinitionData> mConsts;
    };

    class ShaderBuilderDataFragment
    {
    public:
        std::vector<GPUVariableDefinitionData> mFragmentInputs;
        std::vector<GPUVariableDefinitionData> mFragmentOutputs;
        std::vector<GPUVariableDefinitionData> mUniforms;
        std::vector<GPUVariableDefinitionData> mConsts;
    };

    class ShaderBuilderData
    {
    public:
        ShaderBuilderDataCommon mCommonVariables;
        ShaderBuilderDataVertex mVertexVariables;
        ShaderBuilderDataFragment mFragmentVariables;
    };

public:
    MaterialRuntime() = default;
    virtual ~MaterialRuntime() = default;
    void init(PoolHandler<Material> material);
    void init(const MaterialRuntimeData& materialRuntimeData);
    virtual void createVertexShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer,
        const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
        {};
    virtual void createFragmentShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer,
        const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
        {};

protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock();

protected:
    MaterialRuntimeData mMaterialRuntimeData;

public:
    CRGET(MaterialRuntimeData)
};
