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
};

class MaterialInstancedPropertiesGPUData
{
public:
    inline static const GPUVariableDefinitionData mColor{GPUStorage::NONE, GPUBuiltIn::PrimitiveTypes::mVector4, "color"};
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
    void bind(Ptr<GPUShader> shader, bool isWorldSpace, bool isInstanced, Ptr<const Mesh> mesh) const;
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
