#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/Systems/System.hpp"
#include "Graphics/Material/Material.hpp"

class MaterialManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    PoolHandler<Texture> loadTexture(const TextureData& textureData);
    void unloadTexture(PoolHandler<Texture>& texture);
    PoolHandler<Material> createMaterial(const MaterialData& materialData, MaterialRuntime* materialRuntime);
    PoolHandler<Material> createMaterial(const MaterialData& materialData);
    MaterialInstance createMaterialInstance(const PoolHandler<Material>& handler);
    void removeMaterial(PoolHandler<Material>& material);
    const Material& getMaterial(const PoolHandler<Material>& handler) const;

private:
    Pool<Texture> mTextures;
    std::unordered_map<std::string, PoolHandler<Texture>> mTexturesByPath;
    Pool<Material> mMaterials;
};
REGISTER_CLASS(MaterialManager);
