#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Graphics/Material/Material.hpp"

class MaterialManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void update() override;
    PoolHandler<Texture> loadTexture(const TextureData& textureData);
    void unloadTexture(PoolHandler<Texture>& texture);

    template<class T> T_EXTENDS(T, Shader)
    PoolHandler<Material> createMaterial(const MaterialData& materialData)
    {
        // OwnerPtr<T> shader = OwnerPtr<T>::newObject();
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

    void setMaterialInstanceProperties(const Slot& slot, const MaterialInstance& materialInstance);
    const GPUSharedBuffer& getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const;
    Slot requestMaterialInstanceSlot(const PoolHandler<Material>& material);
    void freeMaterialInstanceSlot(const PoolHandler<Material>& material, const Slot& slot);

private:
    void postMaterialCreated(const PoolHandler<Material>& handler);

    class MaterialRenderState
    {
    public:
        ByteBuffer mMaterialPropertiesBlockArray;
        GPUSharedBuffersContainer mGPUSharedBuffersContainer;
        PoolHandler<Material> mMaterial;
        SlotsManager mSlotsManager;
    };

	std::unordered_map<u32, MaterialRenderState> mMaterialRenderStates;

    void initMaterialInstancePropertiesSharedBuffer(const PoolHandler<Material>& material);

private:
    Pool<Texture> mTextures;
    std::unordered_map<HashedString, PoolHandler<Texture>> mTexturesByPath;
    Pool<Material> mMaterials;
};
REGISTER_CLASS(MaterialManager);
