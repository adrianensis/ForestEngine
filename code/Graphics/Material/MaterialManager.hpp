#pragma once

#include "Core/Module.hpp"
#include "Core/Systems/System.hpp"
#include "Graphics/Material/Material.hpp"

class MaterialManager: public System
{
	GENERATE_METADATA(MaterialManager)

public:
    void init();
    Handler loadTexture(const TextureData& textureData);
    Handler createMaterial(const MaterialData& materialData);
    const Material& getMaterial(const Handler& handler) const;
    const Texture& getTexture(const Handler& handler) const;

private:
    inline constexpr static const u32 smMaxMaterials = 500;
    inline constexpr static const u32 smMaxTextures = 1000;
    u32 mMaterialIDCounter = 0;
    u32 mTextureIDCounter = 0;
    std::array<Texture, smMaxTextures> mTextures;
    std::unordered_map<std::string, Handler> mTexturesByPath;
    std::array<Material, smMaxMaterials> mMaterials;

    Handler mNoTextureMaterial;

public:
    GET(NoTextureMaterial)
};
