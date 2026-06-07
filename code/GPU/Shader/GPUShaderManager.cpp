#include "GPU/Shader/GPUShaderManager.hpp"
#include "Core/Assert/Assert.hpp"
#include "GPU/Texture/GPUTexture.hpp"
#include "GPU/Texture/GPUTextureManager.hpp"

void GPUShaderManager::init(GPUTextureManager* gpuTextureManager)
{
    mGPUTextureManager = gpuTextureManager;
}

void GPUShaderManager::terminate()
{
    FOR_MAP(it, mGPUShaderPropertyBlockRenderStates)
    {
        it->second.mGPUUniformBuffersContainer.terminate();
    }

    mGPUShaderPropertyBlockRenderStates.clear();
}

void GPUShaderManager::update()
{
    PROFILER_CPU();

    for(auto& it: mDirtyGPUShaderPropertiesInstances)
    {
        PROFILER_CPU()
        GPUShaderPropertiesInstance* shaderPropertiesInstance = mGPUShaderPropertiesInstances.at(it);
        GPUShader* shader = shaderPropertiesInstance->mShader;
        CHECK_MSG(shader, "Invalid shader!");
        GPUShaderPropertiesBlockID propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockId();

        if(shader->allowInstances())
        {
            PROFILER_CPU_NAMED(allowInstances)
            if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
            {
                if(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlots[shaderPropertiesInstance->mSlot])
                {
                    GPU::u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
                    PROFILER_CPU_NAMED(CopyBuffer)
                    mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.copyBufferAt(shaderPropertiesInstance->mGPUShaderPropertiesBlockBuffer.getByteBuffer(), shaderPropertiesInstance->mSlot * propertiesBlockSizeBytes);
                }
            }
        }
    }
    mDirtyGPUShaderPropertiesInstances.clear();

    FOR_MAP(it, mGPUShaderPropertyBlockRenderStates)
    {
        GPU::ByteBuffer& shaderPropertiesBlockArray = it->second.mGPUShaderPropertiesBlockArray;
        it->second.mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderPropertiesBlockNames::smPropertiesBlockBufferName).setDataArray(shaderPropertiesBlockArray);
    }
}

void GPUShaderManager::postGPUShaderCreated(GPUContext* gpuContext, GPUShader* shader)
{
    initGPUShaderPropertiesInstancePropertiesUniformBuffer(gpuContext, shader);
}

GPUShaderPropertiesInstance* GPUShaderManager::createGPUShaderPropertiesInstance(GPUShader* shader)
{
    PROFILER_CPU()
    GPUShaderPropertiesInstance* instance = mGPUShaderPropertiesInstances.emplace_back( new GPUShaderPropertiesInstance());
    instance->mShader = shader;
    instance->mID = mGPUShaderPropertiesInstances.size() - 1;
    instance->mGPUShaderPropertiesBlockBuffer = shader->getSharedGPUShaderPropertiesBlockBuffer();
    instance->mSlot = requestGPUShaderPropertiesInstanceSlot(shader);

    return instance;
}
void GPUShaderManager::freeGPUShaderPropertiesInstance(GPUShaderPropertiesInstance* shaderPropertiesInstance)
{
    PROFILER_CPU()
    CHECK_MSG(shaderPropertiesInstance->mShader, "Invalid shader!");
    GPUShaderPropertiesBlockID propertiesBlockClassId = shaderPropertiesInstance->mShader->getSharedGPUShaderPropertiesBlockId();

    if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shaderPropertiesInstance->mShader->getGPUShaderData().mAllowInstances)
        {
            mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlots[shaderPropertiesInstance->mSlot] = false;
        }
    }  
}

void GPUShaderManager::initGPUShaderPropertiesInstancePropertiesUniformBuffer(GPUContext* gpuContext, GPUShader* shader)
{
    CHECK_MSG(shader, "Invalid shader!");
    GPU::u32 shaderID = shader->getID();
    GPUShaderPropertiesBlockID propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockId();
    
    if(!mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shader->allowInstances())
        {
            GPU::u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
            if(propertiesBlockSizeBytes > 0)
            {
                PROFILER_CPU()
                mGPUShaderPropertyBlockRenderStates.emplace(propertiesBlockClassId, GPUShaderPropertyBlockRenderState());

                // GPU::u32 size = shader->getGPUShaderData().getMaxInstances();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlots.resize(mInitialInstances);
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.resize(mInitialInstances * propertiesBlockSizeBytes);

                // Reserve index 0 for default shader instance
                GPU::u32 defaultSlot = mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).requestSlot();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.copyBufferAt(shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer(), defaultSlot * propertiesBlockSizeBytes);

                const GPUUniformBufferData& propertiesBlockUniformBufferData = shader->getPropertiesBlockUniformBufferData();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.addUniformBuffer(gpuContext, propertiesBlockUniformBufferData, propertiesBlockSizeBytes * mInitialInstances, false);
            }
        }
    }
}

void GPUShaderManager::setGPUShaderPropertiesInstanceDirty(GPU::u32 id)
{
    PROFILER_CPU()

    GPUShaderPropertiesInstance* shaderPropertiesInstance = mGPUShaderPropertiesInstances.at(id);
    CHECK_MSG(shaderPropertiesInstance, "Invalid shader Instance!");
    GPUShader* shader = shaderPropertiesInstance->mShader;
    CHECK_MSG(shader, "Invalid shader!");
    GPU::u32 shaderID = shader->getID();
    GPUShaderPropertiesBlockID propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockId();

    if(shader->allowInstances())
    {
        if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            CHECK_MSG(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlots[shaderPropertiesInstance->mSlot], "Invalid slot!");
            mDirtyGPUShaderPropertiesInstances.insert(id);
        }
    }
}

const GPUUniformBuffer& GPUShaderManager::getGPUShaderPropertiesGPUUniformBuffer(GPUShader* shader) const
{
    CHECK_MSG(shader, "Invalid shader!");
    GPUShaderPropertiesBlockID propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockId();
    CHECK_MSG(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId), "GPUShader Property Block not found!");
    return mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderPropertiesBlockNames::smPropertiesBlockBufferName);
}

GPU::u32 GPUShaderManager::requestGPUShaderPropertiesInstanceSlot(GPUShader* shader)
{
    PROFILER_CPU()

    CHECK_MSG(shader, "Invalid shader!");
    GPUShaderPropertiesBlockID propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockId();
    
    GPU::u32 slot = 0;
    bool found = false;
    if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {    
        if(shader->getGPUShaderData().mAllowInstances)
        {
            if(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).isEmpty())
            {
                CHECK_MSG(false, "mGPUShaderPropertyBlockRenderStates propertiesBlockClassId mSlotsManager.isEmpty!");

                GPU::u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
                GPU::u32 currentSize = mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlots.size();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlots.resize(currentSize + mInitialInstances);
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.resize(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlots.size() * propertiesBlockSizeBytes);
                // mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderPropertiesBlockNames::smPropertiesBlockBufferName).resizeBytes(propertiesBlockSizeBytes * mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.getSize());
            }

            found = true;
            slot = mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).requestSlot();
        }
        else
        {
            // point to default shader instance
            found = true;
            slot = 0;
        }
    }

    CHECK_MSG(found, "No Material slot available!")

    return slot;
}