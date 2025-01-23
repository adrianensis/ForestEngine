#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Graphics/Shader/Shader.hpp"

class ShaderManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    WeakPtr<GPUTexture> loadTexture(const GPUTextureData& gpuTextureData);

    template<class T> T_EXTENDS(T, Shader)
    WeakPtr<Shader> createShader(const ShaderData& shaderData)
    {
        WeakPtr<Shader> shader = mShaders.emplace_back(OwnerPtr<Shader>::moveCast(OwnerPtr<T>::newObject()));
        shader->init(shaderData, mShaders.size() - 1);
        postShaderCreated(shader);

        return shader;
    }

    WeakPtr<ShaderInstance> createShaderInstance(WeakPtr<Shader> shader);
    void freeShaderInstance(WeakPtr<ShaderInstance> shaderInstance);

    void setShaderInstanceProperties(const WeakPtr<ShaderInstance> shaderInstance);
    void setShaderInstanceDirty(u32 id);

    const GPUUniformBuffer& getShaderPropertiesGPUUniformBuffer(WeakPtr<Shader> shader) const;
    Slot requestShaderInstanceSlot(WeakPtr<Shader> shader);

    const std::unordered_map<HashedString, WeakPtr<GPUTexture>>& getShaderTextureBindings(u32 id) const;
    
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
	
    std::vector<OwnerPtr<GPUTexture>> mTextures;
    std::vector<TextureHandle> mTextureHandles;
    std::unordered_map<HashedString, WeakPtr<GPUTexture>> mTexturesByPath;
	std::unordered_map<u32, std::unordered_map<HashedString, WeakPtr<GPUTexture>>> mTextureBindingsByShader;
    std::vector<OwnerPtr<Shader>> mShaders;
    std::vector<OwnerPtr<ShaderInstance>> mShaderInstances;
    std::unordered_set<u32> mDirtyShaderInstances;
    inline static const u32 mInitialInstances = 2000;
    inline static const u32 mInitialTextures = 300;
};
REGISTER_CLASS(ShaderManager);
