#pragma once

#include "Graphics/Material/MaterialRuntime.hpp"

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

class MaterialRuntimePhong : public MaterialRuntime
{
protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock() override;

    virtual void fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const override;
    virtual void fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const override;

    virtual void generateShaderBuilderData(MaterialRuntime::ShaderBuilderData& shaderBuilderData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const override;
    virtual void registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const override;

    void registerFunctionCalculatePhong(ShaderBuilder& shaderBuilder) const;

protected:
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
};
