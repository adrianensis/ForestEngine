#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

class GPUShader;
class Mesh;

class MaterialInstancedProperties
{
public:
    Vector4 mColor = Vector4(0,0,0,1);
    Vector2 mTextureRegionLeftTop = Vector2(0.0, 0.0);
    Vector2 mTextureRegionSize = Vector2(1.0, 1.0);
    alignas(16) i32 mDepth = 0;
};

class MaterialInstancedPropertiesGPUData
{
public:
    inline static const GPUVariableDefinitionData mColor{GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector4, "color"};
    inline static const GPUVariableDefinitionData mTextureRegionLeftTop{GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector2, "textureRegionLeftTop"};
    inline static const GPUVariableDefinitionData mTextureRegionSize{GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector2, "textureRegionSize"};
    inline static const GPUVariableDefinitionData mDepth{GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mInt, "depth"};
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
    FontData mFontData;
    std::array<std::string, (u32)TextureType::MAX> mTexturePaths;
    std::unordered_map<std::string, TextureAnimation> mTextureAnimations;
    MaterialInstancedProperties mMaterialInstancedProperties;
};

class Material;

class MaterialInstance
{
public:
    Ptr<const Material> mMaterial;
    MaterialInstancedProperties mMaterialInstancedProperties;
};

class Material: public ObjectBase
{
    GENERATE_METADATA(Material)
	DECLARE_SERIALIZATION()

public:
    void init(const MaterialData& materialData, u32 id);
    void addInstancedProperty(const GPUVariableData& gpuVariableData) { mInstancedProperties.push_back(gpuVariableData); }
    void enable() const;
    void disable() const;
    bool hasTexture() const;
    MaterialInstance createMaterialInstance() const;
    
protected:
    virtual void loadTextures();

protected:
    MaterialData mMaterialData;
    std::vector<GPUVariableData> mInstancedProperties;
	GPUStructDefinition mInstancedPropertiesStructDefinition;
    GPUSharedBufferData mInstancedPropertiesSharedBufferData;
    std::array<Ptr<const Texture>, (u32)TextureType::MAX> mTextures;

    u32 mID = 0;

public:
    CRGET(MaterialData)
    CRGET(InstancedPropertiesSharedBufferData)
    CRGET(InstancedPropertiesStructDefinition)
    GET(ID)
};

class MaterialFont: public Material
{
    GENERATE_METADATA(MaterialFont)
protected:
    virtual void loadTextures() override;
};
