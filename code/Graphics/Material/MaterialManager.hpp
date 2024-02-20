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
    PoolHandler<Material> createMaterial(const MaterialData& materialData);
    const Material& getMaterial(const PoolHandler<Material>& handler) const;

private:
    ObjectPool<Texture> mTextures;
    std::unordered_map<std::string, PoolHandler<Texture>> mTexturesByPath;
    ObjectPool<Material> mMaterials;

    PoolHandler<Material> mNoTextureMaterial;

public:
    GET(NoTextureMaterial)
};
