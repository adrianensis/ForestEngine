#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Shader/Shader.hpp"

class MaterialInstance
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
    void update();
    PoolHandler<GPUTexture> loadTexture(const GPUTextureData& gpuTextureData);
    void unloadTexture(PoolHandler<GPUTexture>& texture);

    template<class T> T_EXTENDS(T, Shader)
    PoolHandler<Material> createMaterial(const MaterialData& materialData)
    {
        PoolHandler<Material> handler = mMaterials.allocate();
        handler->init(materialData, handler.getIndex());
        mMaterialToShader.emplace(handler->getID(), OwnerPtr<Shader>::moveCast(OwnerPtr<T>::newObject()));
        mMaterialToShader.at(handler->getID())->init();
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

    const GPUUniformBuffer& getMaterialPropertiesGPUUniformBuffer(const PoolHandler<Material>& material) const;
    Slot requestMaterialInstanceSlot(const PoolHandler<Material>& material);

    const std::unordered_map<HashedString, PoolHandler<GPUTexture>>& getMaterialTextureBindings(const PoolHandler<Material>& handler) const;
    
    WeakPtr<Shader> getMaterialShader(const PoolHandler<Material>& handler) const;

private:
    void postMaterialCreated(const PoolHandler<Material>& handler);
    void loadMaterialTextures(const PoolHandler<Material>& handler);
    void initMaterialInstancePropertiesUniformBuffer(const PoolHandler<Material>& material);

    class MaterialPropertyBlockRenderState
    {
    public:
        ByteBuffer mMaterialPropertiesBlockArray;
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
        SlotsManager mSlotsManager;
    };

	std::unordered_map<u32, ClassId> mMaterialToPropertyBlock;
	std::unordered_map<ClassId, MaterialPropertyBlockRenderState> mMaterialPropertyBlockRenderStates;
	
    std::unordered_map<u32, OwnerPtr<Shader>> mMaterialToShader;

    Pool<GPUTexture> mTextures;
    std::vector<TextureHandle> mTextureHandles;
    std::unordered_map<HashedString, PoolHandler<GPUTexture>> mTexturesByPath;
	std::unordered_map<u32, std::unordered_map<HashedString, PoolHandler<GPUTexture>>> mTextureBindingsByMaterial;
    Pool<Material> mMaterials;
    Pool<MaterialInstance> mMaterialInstances;
    std::unordered_set<u32> mDirtyMaterialInstances;
    inline static const u32 mInitialInstances = 2000;
};
REGISTER_CLASS(MaterialManager);
