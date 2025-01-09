#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Graphics/Shader/Shader.hpp"

class ShaderInstance
{
public:
    Slot mSlot;
    u32 mID = 0;
    WeakPtr<Shader> mShader;
    GenericObjectBuffer mShaderPropertiesBlockBuffer;
    void setDirty();
};
REGISTER_CLASS(ShaderInstance);

class ShaderManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    PoolHandler<GPUTexture> loadTexture(const GPUTextureData& gpuTextureData);
    void unloadTexture(PoolHandler<GPUTexture>& texture);

    template<class T> T_EXTENDS(T, Shader)
    WeakPtr<Shader> createShader(const ShaderData& shaderData)
    {
        WeakPtr<Shader> shader = mShaders.emplace_back(OwnerPtr<Shader>::moveCast(OwnerPtr<T>::newObject()));
        shader->init(shaderData, mShaders.size() - 1);
        postShaderCreated(shader);

        return shader;
    }

    PoolHandler<ShaderInstance> createShaderInstance(WeakPtr<Shader> shader);
    void freeShaderInstance(PoolHandler<ShaderInstance> shaderInstance);

    void setShaderInstanceProperties(const PoolHandler<ShaderInstance> shaderInstance);
    void setShaderInstanceDirty(u32 id);

    const GPUUniformBuffer& getShaderPropertiesGPUUniformBuffer(WeakPtr<Shader> shader) const;
    Slot requestShaderInstanceSlot(WeakPtr<Shader> shader);

    const std::unordered_map<HashedString, PoolHandler<GPUTexture>>& getShaderTextureBindings(u32 id) const;
    
private:
    void postShaderCreated(WeakPtr<Shader> shader);
    void loadShaderTextures(WeakPtr<Shader> shader);
    void initShaderInstancePropertiesUniformBuffer(WeakPtr<Shader> shader);

    class ShaderPropertyBlockRenderState
    {
    public:
        ByteBuffer mShaderPropertiesBlockArray;
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
        SlotsManager mSlotsManager;
    };

	std::unordered_map<ClassId, ShaderPropertyBlockRenderState> mShaderPropertyBlockRenderStates;
	
    Pool<GPUTexture> mTextures;
    std::vector<TextureHandle> mTextureHandles;
    std::unordered_map<HashedString, PoolHandler<GPUTexture>> mTexturesByPath;
	std::unordered_map<u32, std::unordered_map<HashedString, PoolHandler<GPUTexture>>> mTextureBindingsByShader;
    std::vector<OwnerPtr<Shader>> mShaders;
    Pool<ShaderInstance> mShaderInstances;
    std::unordered_set<u32> mDirtyShaderInstances;
    inline static const u32 mInitialInstances = 2000;
};
REGISTER_CLASS(ShaderManager);
