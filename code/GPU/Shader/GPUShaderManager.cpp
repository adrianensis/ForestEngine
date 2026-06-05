#include "GPU/Shader/GPUShaderManager.hpp"
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

    FOR_LIST(it, mDirtyGPUShaderPropertiesInstances)
    {
        GPUShaderPropertiesInstance* instance = mGPUShaderPropertiesInstances.at(*it);
        setGPUShaderPropertiesInstanceProperties(instance);
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
    Core::ClassId propertiesBlockClassId = shaderPropertiesInstance->mShader->getSharedGPUShaderPropertiesBlockClass().getId();

    if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shaderPropertiesInstance->mShader->getGPUShaderData().mAllowInstances)
        {
            mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.freeSlot(shaderPropertiesInstance->mSlot);
        }
    }  
}

void GPUShaderManager::initGPUShaderPropertiesInstancePropertiesUniformBuffer(GPUContext* gpuContext, GPUShader* shader)
{
    CHECK_MSG(shader, "Invalid shader!");
    GPU::u32 shaderID = shader->getID();
    Core::ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();
    
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
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.init(mInitialInstances);
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.resize(mInitialInstances * propertiesBlockSizeBytes);

                // Reserve index 0 for default shader instance
                Core::Slot defaultSlot = mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.requestSlot();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.copyBufferAt(shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer(), defaultSlot.getSlot() * propertiesBlockSizeBytes);

                const GPUUniformBufferData& propertiesBlockUniformBufferData = shader->getPropertiesBlockUniformBufferData();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.addUniformBuffer(gpuContext, propertiesBlockUniformBufferData, propertiesBlockSizeBytes * mInitialInstances, false);
            }
        }
    }
}

void GPUShaderManager::setGPUShaderPropertiesInstanceProperties(const GPUShaderPropertiesInstance* shaderPropertiesInstance)
{
    PROFILER_CPU()

    GPUShader* shader = shaderPropertiesInstance->mShader;
    CHECK_MSG(shader, "Invalid shader!");
    GPU::u32 shaderID = shader->getID();
    Core::ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();

    if(shader->allowInstances())
    {
        PROFILER_CPU_NAMED(allowInstances)
        if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            PROFILER_CPU()
            CHECK_MSG(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(shaderPropertiesInstance->mSlot), "Invalid slot!");
            GPU::u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
            PROFILER_CPU_NAMED(CopyBuffer)
            mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.copyBufferAt(shaderPropertiesInstance->mGPUShaderPropertiesBlockBuffer.getByteBuffer(), shaderPropertiesInstance->mSlot.getSlot() * propertiesBlockSizeBytes);
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
    Core::ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();

    if(shader->allowInstances())
    {
        if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            CHECK_MSG(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(shaderPropertiesInstance->mSlot), "Invalid slot!");
            mDirtyGPUShaderPropertiesInstances.insert(id);
        }
    }
}

const GPUUniformBuffer& GPUShaderManager::getGPUShaderPropertiesGPUUniformBuffer(GPUShader* shader) const
{
    CHECK_MSG(shader, "Invalid shader!");
    Core::ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();
    CHECK_MSG(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId), "GPUShader Property Block not found!");
    return mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderPropertiesBlockNames::smPropertiesBlockBufferName);
}

Core::Slot GPUShaderManager::requestGPUShaderPropertiesInstanceSlot(GPUShader* shader)
{
    PROFILER_CPU()

    CHECK_MSG(shader, "Invalid shader!");
    Core::ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();
    
    Core::Slot slot;
    if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {    
        if(shader->getGPUShaderData().mAllowInstances)
        {
            if(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.isEmpty())
            {
                CHECK_MSG(false, "mGPUShaderPropertyBlockRenderStates propertiesBlockClassId mSlotsManager.isEmpty!");

                GPU::u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.increaseSize(mInitialInstances);
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.resize(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.getSize() * propertiesBlockSizeBytes);
                // mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderPropertiesBlockNames::smPropertiesBlockBufferName).resizeBytes(propertiesBlockSizeBytes * mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.getSize());
            }

            slot = mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.requestSlot();
        }
        else
        {
            // point to default shader instance
            slot.set(0);
        }
    }

    return slot;
}