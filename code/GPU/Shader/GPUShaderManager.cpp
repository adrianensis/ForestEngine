#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/Image/GPUTexture.hpp"
#include "GPU/GPUInstance.hpp"

void GPUShaderManager::init()
{
    mTextureHandles.reserve(mInitialTextures);
    mTextures.reserve(mInitialTextures);
    // INFO: We reserve position 0 to represent NULL
    mTextureHandles.emplace_back();
    mTextures.emplace_back();
}

void GPUShaderManager::terminate()
{
    FOR_MAP(it, mGPUShaderPropertyBlockRenderStates)
    {
        it->second.mGPUUniformBuffersContainer.terminate();
    }

    mGPUShaderPropertyBlockRenderStates.clear();

    FOR_LIST(it, mTextures)
    {
        if(*it)
        {
            (*it)->terminate();
        }
    }
    FOR_LIST(it, mShaders)
    {
        if(*it)
        {
            (*it)->terminate();
        }
    }
}

void GPUShaderManager::update()
{
    PROFILER_CPU();

    FOR_LIST(it, mDirtyGPUShaderPropertiesInstances)
    {
        WeakPtr<GPUShaderPropertiesInstance> instance = mGPUShaderPropertiesInstances.at(*it);
        setGPUShaderPropertiesInstanceProperties(instance);
    }
    mDirtyGPUShaderPropertiesInstances.clear();

    FOR_MAP(it, mGPUShaderPropertyBlockRenderStates)
    {
        ByteBuffer& shaderPropertiesBlockArray = it->second.mGPUShaderPropertiesBlockArray;
        it->second.mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderPropertiesBlockNames::smPropertiesBlockBufferName).setDataArray(shaderPropertiesBlockArray);
    }
}

WeakPtr<GPUTexture> GPUShaderManager::loadTexture(const GPUTextureData& gpuTextureData)
{
	if (!mTexturesByPath.contains(gpuTextureData.mPath))
	{
        PROFILER_CPU()
        WeakPtr<GPUTexture> texture = mTextures.emplace_back(OwnerPtr<GPUTexture>::newObject());
        mTexturesByPath.insert_or_assign(gpuTextureData.mPath, texture);
        texture->init(GET_SYSTEM(GPUInstance).mGPUContext, gpuTextureData, mTextures.size() - 1);
	}

	return mTexturesByPath.at(gpuTextureData.mPath);
}

void GPUShaderManager::postGPUShaderCreated(WeakPtr<GPUShader> shader)
{
    loadGPUShaderTextures(shader);
    initGPUShaderPropertiesInstancePropertiesUniformBuffer(shader);
}

void GPUShaderManager::loadGPUShaderTextures(WeakPtr<GPUShader> shader)
{
    u32 id = shader->getID();
    if(!mTextureBindingsByShader.contains(id))
    {
        PROFILER_CPU()
        mTextureBindingsByShader.emplace(id, std::unordered_map<Core::HashedString, WeakPtr<GPUTexture>>());

        FOR_MAP(it, shader->getGPUShaderData().mGPUShaderTextureBindings.mTextureBindings)
        {
            CHECK_MSG(!it->second.mPath.get().empty(), "texture mPath cannot be empty!");
            GPUTextureData gpuTextureData;
            gpuTextureData.mPath = it->second.mPath;

            if(shader->getGPUShaderData().mIsFont)
            {
                CHECK_MSG(!shader->getGPUShaderData().mFontData.mPath.get().empty(), "mGPUShaderData.mFontData.mPath cannot be empty!");
                gpuTextureData.mIsFont = true;
                gpuTextureData.mFontData = shader->getGPUShaderData().mFontData;
            }

            mTextureBindingsByShader.at(id).insert_or_assign(it->first, GET_SYSTEM(GPUShaderManager).loadTexture(gpuTextureData));
        }
    }
}

const std::unordered_map<Core::HashedString, WeakPtr<GPUTexture>>& GPUShaderManager::getGPUShaderTextureBindings(u32 id) const
{
    return mTextureBindingsByShader.at(id);
}

WeakPtr<GPUShaderPropertiesInstance> GPUShaderManager::createGPUShaderPropertiesInstance(WeakPtr<GPUShader> shader)
{
    PROFILER_CPU()
    WeakPtr<GPUShaderPropertiesInstance> instance = mGPUShaderPropertiesInstances.emplace_back(OwnerPtr<GPUShaderPropertiesInstance>::newObject());
    instance->mShader = shader;
    instance->mID = mGPUShaderPropertiesInstances.size() - 1;
    instance->mGPUShaderPropertiesBlockBuffer = shader->getSharedGPUShaderPropertiesBlockBuffer();
    instance->mSlot = requestGPUShaderPropertiesInstanceSlot(shader);

    return instance;
}
void GPUShaderManager::freeGPUShaderPropertiesInstance(WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance)
{
    PROFILER_CPU()
    CHECK_MSG(shaderPropertiesInstance->mShader.isValid(), "Invalid shader!");
    ClassId propertiesBlockClassId = shaderPropertiesInstance->mShader->getSharedGPUShaderPropertiesBlockClass().getId();

    if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shaderPropertiesInstance->mShader->getGPUShaderData().mAllowInstances)
        {
            mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.freeSlot(shaderPropertiesInstance->mSlot);
        }
    }  
}

void GPUShaderManager::initGPUShaderPropertiesInstancePropertiesUniformBuffer(WeakPtr<GPUShader> shader)
{
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    u32 shaderID = shader->getID();
    ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();
    
    if(!mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shader->allowInstances())
        {
            u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
            if(propertiesBlockSizeBytes > 0)
            {
                PROFILER_CPU()
                mGPUShaderPropertyBlockRenderStates.emplace(propertiesBlockClassId, GPUShaderPropertyBlockRenderState());

                // u32 size = shader->getGPUShaderData().getMaxInstances();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.init(mInitialInstances);
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.resize(mInitialInstances * propertiesBlockSizeBytes);

                // Reserve index 0 for default shader instance
                Slot defaultSlot = mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.requestSlot();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.copyBufferAt(shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer(), defaultSlot.getSlot() * propertiesBlockSizeBytes);

                const GPUUniformBufferData& propertiesBlockUniformBufferData = shader->getPropertiesBlockUniformBufferData();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.addUniformBuffer(propertiesBlockUniformBufferData, propertiesBlockSizeBytes * mInitialInstances, false);
            }
        }
    }
}

void GPUShaderManager::setGPUShaderPropertiesInstanceProperties(WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance)
{
    PROFILER_CPU()

    WeakPtr<GPUShader> shader = shaderPropertiesInstance->mShader;
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    u32 shaderID = shader->getID();
    ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();

    if(shader->allowInstances())
    {
        PROFILER_CPU_NAMED(allowInstances)
        if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            PROFILER_CPU()
            CHECK_MSG(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(shaderPropertiesInstance->mSlot), "Invalid slot!");
            u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
            PROFILER_CPU_NAMED(CopyBuffer)
            mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.copyBufferAt(shaderPropertiesInstance->mGPUShaderPropertiesBlockBuffer.getByteBuffer(), shaderPropertiesInstance->mSlot.getSlot() * propertiesBlockSizeBytes);
        }
    }
}

void GPUShaderManager::setGPUShaderPropertiesInstanceDirty(u32 id)
{
    PROFILER_CPU()

    WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance = mGPUShaderPropertiesInstances.at(id);
    CHECK_MSG(shaderPropertiesInstance.isValid(), "Invalid shader Instance!");
    WeakPtr<GPUShader> shader = shaderPropertiesInstance->mShader;
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    u32 shaderID = shader->getID();
    ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();

    if(shader->allowInstances())
    {
        if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            CHECK_MSG(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(shaderPropertiesInstance->mSlot), "Invalid slot!");
            mDirtyGPUShaderPropertiesInstances.insert(id);
        }
    }
}

const GPUUniformBuffer& GPUShaderManager::getGPUShaderPropertiesGPUUniformBuffer(WeakPtr<GPUShader> shader) const
{
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();
    CHECK_MSG(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId), "GPUShader Property Block not found!");
    return mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderPropertiesBlockNames::smPropertiesBlockBufferName);
}

Slot GPUShaderManager::requestGPUShaderPropertiesInstanceSlot(WeakPtr<GPUShader> shader)
{
    PROFILER_CPU()

    CHECK_MSG(shader.isValid(), "Invalid shader!");
    ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();
    
    Slot slot;
    if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {    
        if(shader->getGPUShaderData().mAllowInstances)
        {
            if(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.isEmpty())
            {
                CHECK_MSG(false, "mGPUShaderPropertyBlockRenderStates propertiesBlockClassId mSlotsManager.isEmpty!");

                u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
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