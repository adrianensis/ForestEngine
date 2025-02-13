#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPUShaderManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    WeakPtr<GPUTexture> loadTexture(const GPUTextureData& gpuTextureData);

    template<class T, class P> T_EXTENDS(T, GPUShader)
    WeakPtr<GPUShader> createShader(const ShaderData& shaderData, const P& propertiesBlockDefault)
    {
        WeakPtr<GPUShader> shader = mShaders.emplace_back(OwnerPtr<GPUShader>::moveCast(OwnerPtr<T>::newObject()));
        GenericObjectBuffer propertiesBlockDefaultBuffer;
        propertiesBlockDefaultBuffer.set<P>();
        propertiesBlockDefaultBuffer.get<P>() = propertiesBlockDefault;
        shader->init(shaderData, propertiesBlockDefaultBuffer, mShaders.size() - 1);
        postShaderCreated(shader);

        return shader;
    }

    WeakPtr<ShaderPropertiesInstance> createShaderPropertiesInstance(WeakPtr<GPUShader> shader);
    void freeShaderPropertiesInstance(WeakPtr<ShaderPropertiesInstance> shaderPropertiesInstance);

    void setShaderPropertiesInstanceProperties(const WeakPtr<ShaderPropertiesInstance> shaderPropertiesInstance);
    void setShaderPropertiesInstanceDirty(u32 id);

    const GPUUniformBuffer& getShaderPropertiesGPUUniformBuffer(WeakPtr<GPUShader> shader) const;
    Slot requestShaderPropertiesInstanceSlot(WeakPtr<GPUShader> shader);

    const std::unordered_map<HashedString, WeakPtr<GPUTexture>>& getShaderTextureBindings(u32 id) const;
    
private:
    void postShaderCreated(WeakPtr<GPUShader> shader);
    void loadShaderTextures(WeakPtr<GPUShader> shader);
    void initShaderPropertiesInstancePropertiesUniformBuffer(WeakPtr<GPUShader> shader);

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
    std::vector<OwnerPtr<GPUShader>> mShaders;
    std::vector<OwnerPtr<ShaderPropertiesInstance>> mShaderPropertiesInstances;
    std::unordered_set<u32> mDirtyShaderPropertiesInstances;
    inline static const u32 mInitialInstances = 20000;
    inline static const u32 mInitialTextures = 300;
};
REGISTER_CLASS(GPUShaderManager);
