#pragma once

#include "Core/Core.hpp"
#include "GPU/Shader/GPUShader.hpp"
#include "GPU/Core/GPUByteBuffer.hpp"
#include "GPU/Core/GPUGenericObject.hpp"

class GPUShaderManager
{
public:
    void init(GPUTextureManager* gpuTextureManager);
    void terminate();
    void update();

    void setGPUShaderPropertiesInstanceDirty(GPU::u32 id);

    template<class T, class P> T_EXTENDS(T, GPUShader)
    GPUShader* createShader(GPUContext* gpuContext, const GPUShaderData& shaderData, const P& propertiesBlock)
    {
        GPUShader* shader = mShaders.emplace_back(new T());
        GPU::GenericObjectBuffer propertiesBlockDefaultBuffer;
        propertiesBlockDefaultBuffer.set<P>();
        propertiesBlockDefaultBuffer.get<P>() = propertiesBlock;
        shader->init(gpuContext, this, shaderData, propertiesBlockDefaultBuffer, mShaders.size() - 1);
        postGPUShaderCreated(gpuContext, shader);
        
        return shader;
    }
    
    const GPUUniformBuffer& getGPUShaderPropertiesGPUUniformBuffer(GPUShader* shader) const;
    GPUShaderPropertiesInstance* createGPUShaderPropertiesInstance(GPUShader* shader);
    void freeGPUShaderPropertiesInstance(GPUShaderPropertiesInstance* shaderPropertiesInstance);
    
    void setGPUShaderPropertiesInstanceProperties(const GPUShaderPropertiesInstance* shaderPropertiesInstance);
    
    Core::Slot requestGPUShaderPropertiesInstanceSlot(GPUShader* shader);
    
private:
    void postGPUShaderCreated(GPUContext* gpuContext, GPUShader* shader);
    void initGPUShaderPropertiesInstancePropertiesUniformBuffer(GPUContext* gpuContext, GPUShader* shader);

    class GPUShaderPropertyBlockRenderState
    {
    public:
        GPU::ByteBuffer mGPUShaderPropertiesBlockArray;
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
        Core::SlotsManager mSlotsManager;
    };

	std::unordered_map<Core::ClassId, GPUShaderPropertyBlockRenderState> mGPUShaderPropertyBlockRenderStates;
	
    std::vector<GPUShader*> mShaders;
    std::vector<GPUShaderPropertiesInstance*> mGPUShaderPropertiesInstances;
    std::unordered_set<GPU::u32> mDirtyGPUShaderPropertiesInstances;
    inline static const GPU::u32 mInitialInstances = 20000;
    GPUTextureManager* mGPUTextureManager = nullptr;
};

