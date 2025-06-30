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
    WeakPtr<GPUShader> createShader(const GPUShaderData& shaderData, const P& propertiesBlockDefault)
    {
        WeakPtr<GPUShader> shader = mShaders.emplace_back(OwnerPtr<GPUShader>::moveCast(OwnerPtr<T>::newObject()));
        GenericObjectBuffer propertiesBlockDefaultBuffer;
        propertiesBlockDefaultBuffer.set<P>();
        propertiesBlockDefaultBuffer.get<P>() = propertiesBlockDefault;
        shader->init(shaderData, propertiesBlockDefaultBuffer, mShaders.size() - 1);
        postGPUShaderCreated(shader);

        return shader;
    }

    WeakPtr<GPUShaderPropertiesInstance> createGPUShaderPropertiesInstance(WeakPtr<GPUShader> shader);
    void freeGPUShaderPropertiesInstance(WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance);

    void setGPUShaderPropertiesInstanceProperties(const WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance);
    void setGPUShaderPropertiesInstanceDirty(u32 id);

    const GPUUniformBuffer& getGPUShaderPropertiesGPUUniformBuffer(WeakPtr<GPUShader> shader) const;
    Slot requestGPUShaderPropertiesInstanceSlot(WeakPtr<GPUShader> shader);

    const std::unordered_map<Core::HashedString, WeakPtr<GPUTexture>>& getGPUShaderTextureBindings(u32 id) const;
    
private:
    void postGPUShaderCreated(WeakPtr<GPUShader> shader);
    void loadGPUShaderTextures(WeakPtr<GPUShader> shader);
    void initGPUShaderPropertiesInstancePropertiesUniformBuffer(WeakPtr<GPUShader> shader);

    class GPUShaderPropertyBlockRenderState
    {
    public:
        ByteBuffer mGPUShaderPropertiesBlockArray;
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
        SlotsManager mSlotsManager;
    };

	std::unordered_map<ClassId, GPUShaderPropertyBlockRenderState> mGPUShaderPropertyBlockRenderStates;
	
    std::vector<OwnerPtr<GPUTexture>> mTextures;
    std::vector<TextureHandle> mTextureHandles;
    std::unordered_map<Core::HashedString, WeakPtr<GPUTexture>> mTexturesByPath;
	std::unordered_map<u32, std::unordered_map<Core::HashedString, WeakPtr<GPUTexture>>> mTextureBindingsByShader;
    std::vector<OwnerPtr<GPUShader>> mShaders;
    std::vector<OwnerPtr<GPUShaderPropertiesInstance>> mGPUShaderPropertiesInstances;
    std::unordered_set<u32> mDirtyGPUShaderPropertiesInstances;
    inline static const u32 mInitialInstances = 20000;
    inline static const u32 mInitialTextures = 300;
};
REGISTER_CLASS(GPUShaderManager);
