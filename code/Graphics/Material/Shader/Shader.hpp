#pragma once

#include "Engine/Minimal.hpp"
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

class ShaderData
{
public:
    PoolHandler<Material> mMaterial;
    GPUStructDefinition mPropertiesBlockStructDefinition;
    GPUSharedBufferData mPropertiesBlockSharedBufferData;
    std::unordered_map<HashedString, PoolHandler<Texture>> mTextures;
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
    void init(PoolHandler<Material> material);
    void init(const ShaderData& shaderData);
    void terminate();

    void enable() const;
    void disable() const;
    bool hasTexture(HashedString bindingName) const;
    bool hasFramebufferBinding(HashedString bindingName) const;

    void bindTextures(Ptr<GPUProgram> gpuProgram) const;
    void addFramebufferBinding(const FramebufferBinding& framebufferBinding);

    virtual void createVertexShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer,
        const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
        {};
    virtual void createFragmentShader(ShaderBuilder& shaderBuilder,
        const GPUVertexBuffersContainer& gpuVertexBuffersContainer,
        const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
        {};

protected:
    void loadTextures();
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock();

protected:
    ShaderData mShaderData;

public:
    CRGET(ShaderData)
};
