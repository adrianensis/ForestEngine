#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUProgram.hpp"

class Material;

class FramebufferBinding
{
public:
    HashedString mSamplerName;
    u32 mTextureID = 0;
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
};

class TextureBinding
{
public:
    HashedString mPath;
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
};

class TextureBindingNames
{
public:
    inline static const HashedString smBaseColor = "BaseColor";
};

class ShaderPropertiesBlockNames
{
public:
    inline static const HashedString smPropertiesBlockStructName = "propertiesBlockStruct";
    inline static const HashedString smPropertiesBlockBufferName = "PropertiesBlock";
    inline static const HashedString smPropertiesBlockInstanceName = "propertiesBlock";
    inline static const HashedString smPropertiesBlockArrayName = "propertiesBlockArray";
};

class ShaderData
{
public:
    PoolHandler<Material> mMaterial;
    GPUStructDefinition mPropertiesBlockStructDefinition;
    GPUSharedBufferData mPropertiesBlockSharedBufferData;
    std::unordered_set<HashedString> mTextures;
    std::unordered_map<HashedString, FramebufferBinding> mFramebufferBindings;
};

class Shader
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
    Shader() = default;
    virtual ~Shader() = default;
    void init();
    void init(const ShaderData& shaderData);
    void terminate();

    void enable() const;
    void disable() const;
    bool hasFramebufferBinding(HashedString bindingName) const;

    void bindTextures(Ptr<GPUProgram> gpuProgram, const std::unordered_map<HashedString, PoolHandler<Texture>>& textures) const;
    void addFramebufferBinding(const FramebufferBinding& framebufferBinding);

    virtual void createVertexShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
        {};
    virtual void createFragmentShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
        {};

protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock();
    virtual void registerTextures() {};

protected:
    ShaderData mShaderData;

public:
    CRGET(ShaderData)
};
REGISTER_CLASS(Shader)