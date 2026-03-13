#pragma once

#include "Core/Core.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPUShaderManager
{
public:
    void init(GPUTextureManager* gpuTextureManager);
    void terminate();
    void update();

    void setGPUShaderPropertiesInstanceDirty(Core::u32 id);

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
    
    const GPUUniformBuffer& getGPUShaderPropertiesGPUUniformBuffer(Core::WeakPtr<GPUShader> shader) const;
    Core::WeakPtr<GPUShaderPropertiesInstance> createGPUShaderPropertiesInstance(Core::WeakPtr<GPUShader> shader);
    void freeGPUShaderPropertiesInstance(Core::WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance);
    
    void setGPUShaderPropertiesInstanceProperties(const Core::WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance);
    
    Core::Slot requestGPUShaderPropertiesInstanceSlot(Core::WeakPtr<GPUShader> shader);
    
private:
    void postGPUShaderCreated(GPUContext* gpuContext, Core::WeakPtr<GPUShader> shader);
    void initGPUShaderPropertiesInstancePropertiesUniformBuffer(GPUContext* gpuContext, Core::WeakPtr<GPUShader> shader);

    class GPUShaderPropertyBlockRenderState
    {
    public:
        Core::ByteBuffer mGPUShaderPropertiesBlockArray;
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
        Core::SlotsManager mSlotsManager;
    };

	std::unordered_map<Core::ClassId, GPUShaderPropertyBlockRenderState> mGPUShaderPropertyBlockRenderStates;
	
    std::vector<Core::OwnerPtr<GPUShader>> mShaders;
    std::vector<Core::OwnerPtr<GPUShaderPropertiesInstance>> mGPUShaderPropertiesInstances;
    std::unordered_set<Core::u32> mDirtyGPUShaderPropertiesInstances;
    inline static const Core::u32 mInitialInstances = 20000;
    GPUTextureManager* mGPUTextureManager = nullptr;
};

