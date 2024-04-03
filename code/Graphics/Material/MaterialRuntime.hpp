#pragma once

#include "Core/Object/ObjectBase.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"

class Material;

class MaterialLightingModelPhong
{
public:
    Vector3 mAmbient = Vector3(1,1,1);
    alignas(16) Vector3 mDiffuse = Vector3(0,0,0);
    alignas(16) Vector3 mSpecular = Vector3(0.2f,0.2f,0.2f);
    alignas(16) Vector3 mShininess = Vector3(32,0,0);
};

class MaterialPropertiesBlockPhong
{
public:
    MaterialLightingModelPhong mMaterialLightingModelPhong;
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
    void init(PoolHandler<Material> material);
    void createVertexShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;
    void createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;

protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock();

    void vertexShaderCalculateBoneMatrix(ShaderBuilder& shaderBuilder) const;
    void vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const;
    void vertexShaderCalculateNormalOutput(ShaderBuilder& shaderBuilder) const;
    virtual void vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const;
    void vertexShaderCalculateVertexColorOutput(ShaderBuilder& shaderBuilder) const;
    void vertexShaderCalculateProjectionViewMatrix(ShaderBuilder& shaderBuilder) const;
    void vertexShaderCalculateInstanceIdOutput(ShaderBuilder& shaderBuilder) const;

    void fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const;
    void fragmentShaderTexture(ShaderBuilder& shaderBuilder) const;
    void fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const;
    void fragmentShaderAlphaDiscard(ShaderBuilder& shaderBuilder) const;

    ShaderBuilderData generateShaderBuilderData(const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;
    void registerVertexShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;
    void registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;

    void registerFunctionCalculateBoneTransform(ShaderBuilder& shaderBuilder) const;
    void registerFunctionCalculatePhong(ShaderBuilder& shaderBuilder) const;

protected:
    PoolHandler<Material> mMaterial;
	GPUStructDefinition mLightingModelStructDefinition = 
    {
        "materialLightingModel",
        {
            {GPUBuiltIn::PrimitiveTypes::mVector3, "Ambient"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "Diffuse"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "Specular"},
            {GPUBuiltIn::PrimitiveTypes::mVector3, "Shininess"},
        }
    };

    GPUStructDefinition mPropertiesBlockStructDefinition;
    GPUSharedBufferData mPropertiesBlockSharedBufferData;

public:
    CRGET(PropertiesBlockSharedBufferData)
    CRGET(PropertiesBlockStructDefinition)
};
