#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Material/Texture.hpp"

class GPUShader;
class Mesh;

class MaterialData
{
public:
    std::array<std::string, (u32)TextureType::MAX> mTexturePaths;
	bool mAlphaEnabled = true;
	bool mReceiveLight = true;
	bool mUseVertexColor = false;
	bool mUseNormals = true;
	bool mUseModelMatrix = true;
	bool mUseColorAsTint = false;
    bool mIsSkinned = false;
    bool mCreateMipMap = true;
    FontData mFontData;
	Vector4 mBaseColor = Vector4(0,0,0,1);
    std::unordered_map<std::string, TextureAnimation> mTextureAnimations;
};

class Material: public ObjectBase
{
    GENERATE_METADATA(Material)
	DECLARE_SERIALIZATION()

public:
    void init(const MaterialData& materialData, u32 id);
    void bind(Ptr<GPUShader> shader, bool isWorldSpace, bool isInstanced, Ptr<const Mesh> mesh) const;
    bool hasTexture() const;

protected:
    virtual void loadTextures();

protected:
    MaterialData mMaterialData;
    std::array<Ptr<const Texture>, (u32)TextureType::MAX> mTextures;

    u32 mID = 0;

public:
    CRGET(MaterialData)
    GET(ID)
};

class MaterialFont: public Material
{
    GENERATE_METADATA(MaterialFont)
protected:
    virtual void loadTextures() override;
};
