#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"

class GPUProgram;
class Mesh;

class MaterialInstancedProperties
{
public:
    Vector4 mColor = Vector4(0,0,0,1);
    Vector2 mTextureRegionLeftTop = Vector2(0.0, 0.0);
    Vector2 mTextureRegionSize = Vector2(1.0, 1.0);
    alignas(16) i32 mDepth = 0;
};

class DefaultMaterialInstancedPropertiesGPUData
{
public:
    inline static const std::vector<GPUStructDefinition::GPUStructVariable> smDefaultInstancedProperties = 
    {
        {GPUBuiltIn::PrimitiveTypes::mVector4, "color"},
        {GPUBuiltIn::PrimitiveTypes::mVector2, "textureRegionLeftTop"},
        {GPUBuiltIn::PrimitiveTypes::mVector2, "textureRegionSize"},
        {GPUBuiltIn::PrimitiveTypes::mInt, "depth"},
    };

	inline static const GPUStructDefinition smDefaultInstancedPropertiesStructDefinition =
    {
        "instancedPropertiesStruct",
        {
            smDefaultInstancedProperties
        }
    };

    inline static const GPUDataType smDefaultInstancedPropertiesStructDataType = {smDefaultInstancedPropertiesStructDefinition.mName, smDefaultInstancedPropertiesStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    
    inline static const GPUSharedBufferData smDefaultInstancedPropertiesSharedBufferData =
    {
        GPUBufferType::STORAGE,
        {
            {{GPUStorage::UNIFORM, smDefaultInstancedPropertiesStructDataType, "instancedPropertiesArray"}, "", " "}
        },
        "InstancedProperties",
        "instancedProperties"
    };
};

class MaterialData
{
public:
	bool mAlphaEnabled = true;
	bool mReceiveLight = true;
	bool mUseVertexColor = false;
	bool mUseNormals = true;
	bool mUseModelMatrix = true;
	bool mUseColorAsTint = false;
	bool mUseDepth = false;
    bool mIsSkinned = false;
    bool mCreateMipMap = true;
    bool mIsFont = false;
    FontData mFontData;
    std::array<std::string, (u32)TextureType::MAX> mTexturePaths;
    std::unordered_map<std::string, TextureAnimation> mTextureAnimations;
    MaterialInstancedProperties mMaterialInstancedProperties;
};

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

class Material;

class MaterialInstance
{
public:
    // PoolHandler<Material> mMaterial;
    MaterialInstancedProperties mMaterialInstancedProperties;
};

class Material: public ObjectBase
{
    GENERATE_METADATA(Material)
	DECLARE_SERIALIZATION()

public:
    void init(const MaterialData& materialData, u32 id);
    void terminate();
    virtual void onPoolFree() override { terminate(); };
    void enable() const;
    void disable() const;
    bool hasTexture() const;
    MaterialInstance createMaterialInstance() const;
    
    void createVertexShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;
    void createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const;

protected:
    virtual void loadTextures();

    void vertexShaderCalculateBoneMatrix(ShaderBuilder& shaderBuilder) const;
    void vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const;
    void vertexShaderCalculateNormalOutput(ShaderBuilder& shaderBuilder) const;
    void vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const;
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
    MaterialData mMaterialData;
	GPUStructDefinition mInstancedPropertiesStructDefinition;
    GPUSharedBufferData mInstancedPropertiesSharedBufferData;
    std::array<PoolHandler<Texture>, (u32)TextureType::MAX> mTextures;

    u32 mID = 0;

public:
    CRGET(MaterialData)
    CRGET(InstancedPropertiesSharedBufferData)
    CRGET(InstancedPropertiesStructDefinition)
    GET(ID)
};