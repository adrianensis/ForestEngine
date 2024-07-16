#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Graphics/Material/Material.hpp"

class MaterialInstance: public ObjectBase
{
public:
    Slot mSlot;
    u32 mID = 0;
    PoolHandler<Material> mMaterial;
    GenericObjectBuffer mMaterialPropertiesBlockBuffer;
    void setDirty();
};
REGISTER_CLASS(MaterialInstance);

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

    PoolHandler<MaterialInstance> createMaterialInstance(const PoolHandler<Material>& handler);
    void freeMaterialInstance(PoolHandler<MaterialInstance> materialInstance);
    void removeMaterial(PoolHandler<Material>& material);
    const Material& getMaterial(const PoolHandler<Material>& handler) const;
    PoolHandler<Material> getMaterialHandler(u32 id) const;
    const Material& getMaterial(u32 id) const;

    void setMaterialInstanceProperties(const PoolHandler<MaterialInstance> materialInstance);
    void setMaterialInstanceDirty(u32 id);
    const GPUSharedBuffer& getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const;
    Slot requestMaterialInstanceSlot(const PoolHandler<Material>& material);

    const std::unordered_map<HashedString, PoolHandler<Texture>>& getMaterialTextureBindings(const PoolHandler<Material>& handler) const;

private:
    void postMaterialCreated(const PoolHandler<Material>& handler);
    void loadMaterialTextures(const PoolHandler<Material>& handler);

    class MaterialPropertyBlockRenderState
    {
    public:
        ByteBuffer mMaterialPropertiesBlockArray;
        GPUSharedBuffersContainer mGPUSharedBuffersContainer;
        SlotsManager mSlotsManager;
    };

	std::unordered_map<u32, ClassId> mMaterialToPropertyBlock;
	std::unordered_map<ClassId, MaterialPropertyBlockRenderState> mMaterialPropertyBlockRenderStates;

    void initMaterialInstancePropertiesSharedBuffer(const PoolHandler<Material>& material);

    Pool<Texture> mTextures;
    std::vector<TextureHandle> mTextureHandles;
    std::unordered_map<HashedString, PoolHandler<Texture>> mTexturesByPath;
	std::unordered_map<u32, std::unordered_map<HashedString, PoolHandler<Texture>>> mTextureBindingsByMaterial;
    Pool<Material> mMaterials;
    Pool<MaterialInstance> mMaterialInstances;
    std::unordered_set<u32> mDirtyMaterialInstances;
    inline static const u32 mInitialInstances = 2000;
};
REGISTER_CLASS(MaterialManager);
