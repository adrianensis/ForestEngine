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

    template<class T, class P>
    requires std::derived_from<T, GPUShader>
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
    
    GPU::u32 requestGPUShaderPropertiesInstanceSlot(GPUShader* shader);
    
private:
    void postGPUShaderCreated(GPUContext* gpuContext, GPUShader* shader);
    void initGPUShaderPropertiesInstancePropertiesUniformBuffer(GPUContext* gpuContext, GPUShader* shader);

    class GPUShaderPropertyBlockRenderState
    {
    public:
        GPU::ByteBuffer mGPUShaderPropertiesBlockArray;
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
        std::vector<bool> mSlots;
        GPU::u32 mUsedSlots = 0;

        GPU::u32 requestSlot()
        {   GPU::u32 slot = 0;

            bool found = false;
            FOR_RANGE(i, 0, mSlots.size())
            {
                if(!mSlots[i])
                {
                    slot = i;
                    found = true;
                    break;
                }
            }

            CHECK_MSG(found, "No slot avaliable for renderer.")

            mSlots[slot] = true;
            mUsedSlots++;
            return slot;
        }
        void freeSlot(GPU::u32 slot)
        {
            if(mSlots[slot])
            {
                mSlots[slot] = false;
                mUsedSlots--;
            }
        }
        bool isEmpty()
        {
            return mUsedSlots == mSlots.size();
        }
    };

	std::unordered_map<GPUShaderPropertiesBlockID, GPUShaderPropertyBlockRenderState> mGPUShaderPropertyBlockRenderStates;
	
    std::vector<GPUShader*> mShaders;
    std::vector<GPUShaderPropertiesInstance*> mGPUShaderPropertiesInstances;
    std::unordered_set<GPU::u32> mDirtyGPUShaderPropertiesInstances;
    inline static const GPU::u32 mInitialInstances = 20000;
    GPUTextureManager* mGPUTextureManager = nullptr;
};

