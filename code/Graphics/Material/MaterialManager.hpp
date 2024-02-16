#pragma once

#include "Core/Module.hpp"
#include "Core/Systems/System.hpp"
#include "Graphics/Material/Material.hpp"

class MaterialManager: public System
{
	GENERATE_METADATA(MaterialManager)

public:
    void init();
    PoolHandler<Texture> loadTexture(const TextureData& textureData);
    Handler createMaterial(const MaterialData& materialData);
    const Material& getMaterial(const Handler& handler) const;

private:
    inline constexpr static const u32 smMaxMaterials = 500;
    u32 mMaterialIDCounter = 0;
    ObjectPool<Texture> mTextures;
    std::unordered_map<std::string, PoolHandler<Texture>> mTexturesByPath;
    std::array<Material, smMaxMaterials> mMaterials;

    Handler mNoTextureMaterial;

public:
    GET(NoTextureMaterial)
};
