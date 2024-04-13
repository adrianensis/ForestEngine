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

    template<class T> T_EXTENDS(T, MaterialRuntime)
    PoolHandler<Material> createMaterial(const MaterialData& materialData)
    {
        // OwnerPtr<T> materialRuntime = OwnerPtr<T>::newObject();
        PoolHandler<Material> handler = mMaterials.allocate();
        handler->init<T>(materialData, handler.getIndex());
        postMaterialCreated(handler);

        return handler;
    }

    MaterialInstance createMaterialInstance(const PoolHandler<Material>& handler);
    void removeMaterial(PoolHandler<Material>& material);
    const Material& getMaterial(const PoolHandler<Material>& handler) const;
    PoolHandler<Material> getMaterialHandler(u32 id) const;
    const Material& getMaterial(u32 id) const;

private:
    void postMaterialCreated(const PoolHandler<Material>& handler);

private:
    Pool<Texture> mTextures;
    std::unordered_map<std::string, PoolHandler<Texture>> mTexturesByPath;
    Pool<Material> mMaterials;
};
REGISTER_CLASS(MaterialManager);
