#pragma once

#include "Core/Core.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPUShaderManager
{
public:
    void init();
    void terminate();
    void update();
    Core::WeakPtr<GPUTexture> loadTexture(GPUContext* gpuContext, const GPUTextureData& gpuTextureData);

    template<class T, class P> T_EXTENDS(T, GPUShader)
    Core::WeakPtr<GPUShader> createShader(GPUContext* gpuContext, const GPUShaderData& shaderData, const P& propertiesBlock)
    {
        Core::WeakPtr<GPUShader> shader = mShaders.emplace_back(Core::OwnerPtr<GPUShader>::moveCast(Core::OwnerPtr<T>::newObject()));
        Core::GenericObjectBuffer propertiesBlockDefaultBuffer;
        propertiesBlockDefaultBuffer.set<P>();
        propertiesBlockDefaultBuffer.get<P>() = propertiesBlock;
        shader->init(gpuContext, this, shaderData, propertiesBlockDefaultBuffer, mShaders.size() - 1);
        postGPUShaderCreated(gpuContext, shader);

        return shader;
    }

    Core::WeakPtr<GPUShaderPropertiesInstance> createGPUShaderPropertiesInstance(Core::WeakPtr<GPUShader> shader);
    void freeGPUShaderPropertiesInstance(Core::WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance);

    void setGPUShaderPropertiesInstanceProperties(const Core::WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance);
    void setGPUShaderPropertiesInstanceDirty(Core::u32 id);

    const GPUUniformBuffer& getGPUShaderPropertiesGPUUniformBuffer(Core::WeakPtr<GPUShader> shader) const;
    Core::Slot requestGPUShaderPropertiesInstanceSlot(Core::WeakPtr<GPUShader> shader);

    const std::unordered_map<Core::HashedString, Core::WeakPtr<GPUTexture>>& getGPUShaderTextureBindings(Core::u32 id) const;
    
private:
    void postGPUShaderCreated(GPUContext* gpuContext, Core::WeakPtr<GPUShader> shader);
    void loadGPUShaderTextures(GPUContext* gpuContext, Core::WeakPtr<GPUShader> shader);
    void initGPUShaderPropertiesInstancePropertiesUniformBuffer(GPUContext* gpuContext, Core::WeakPtr<GPUShader> shader);

    class GPUShaderPropertyBlockRenderState
    {
    public:
        Core::ByteBuffer mGPUShaderPropertiesBlockArray;
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
        Core::SlotsManager mSlotsManager;
    };

	std::unordered_map<Core::ClassId, GPUShaderPropertyBlockRenderState> mGPUShaderPropertyBlockRenderStates;
	
    std::vector<Core::OwnerPtr<GPUTexture>> mTextures;
    std::vector<TextureHandle> mTextureHandles;
    std::unordered_map<Core::HashedString, Core::WeakPtr<GPUTexture>> mTexturesByPath;
	std::unordered_map<Core::u32, std::unordered_map<Core::HashedString, Core::WeakPtr<GPUTexture>>> mTextureBindingsByShader;
    std::vector<Core::OwnerPtr<GPUShader>> mShaders;
    std::vector<Core::OwnerPtr<GPUShaderPropertiesInstance>> mGPUShaderPropertiesInstances;
    std::unordered_set<Core::u32> mDirtyGPUShaderPropertiesInstances;
    inline static const Core::u32 mInitialInstances = 20000;
    inline static const Core::u32 mInitialTextures = 300;
};

