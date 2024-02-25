#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/Systems/System.hpp"
#include "Graphics/Material/Material.hpp"

class MaterialManager: public System
{
	

public:
    void init();
    void terminate();
    PoolHandler<Texture> loadTexture(const TextureData& textureData);
    void unloadTexture(PoolHandler<Texture>& texture);
    PoolHandler<Material> createMaterial(const MaterialData& materialData);
    void removeMaterial(PoolHandler<Material>& material);
    const Material& getMaterial(const PoolHandler<Material>& handler) const;

private:
    Pool<Texture> mTextures;
    std::unordered_map<std::string, PoolHandler<Texture>> mTexturesByPath;
    Pool<Material> mMaterials;

    PoolHandler<Material> mNoTextureMaterial;

public:
    GET(NoTextureMaterial)
};
REGISTER_CLASS(MaterialManager);
