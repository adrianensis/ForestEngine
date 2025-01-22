#include "Graphics/Shader/ShaderManager.hpp"
#include "GPU/Image/GPUTexture.hpp"
#include "GPU/GPUInstance.hpp"

void ShaderInstance::setDirty()
{
    PROFILER_CPU();
    GET_SYSTEM(ShaderManager).setShaderInstanceDirty(mID);
}

void ShaderManager::init()
{
	LOG_TRACE()
    mTextureHandles.reserve(mInitialTextures);
    mTextures.reserve(mInitialTextures);
    // INFO: We reserve position 0 to represent NULL
    mTextureHandles.emplace_back();
    mTextures.emplace_back();
}

void ShaderManager::terminate()
{
    FOR_MAP(it, mShaderPropertyBlockRenderStates)
    {
        it->second.mGPUUniformBuffersContainer.terminate();
    }

    mShaderPropertyBlockRenderStates.clear();

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

void ShaderManager::update()
{
    PROFILER_CPU();

    FOR_LIST(it, mDirtyShaderInstances)
    {
        WeakPtr<ShaderInstance> instance = mShaderInstances.at(*it);
        setShaderInstanceProperties(instance);
    }
    mDirtyShaderInstances.clear();

    FOR_MAP(it, mShaderPropertyBlockRenderStates)
    {
        ByteBuffer& shaderPropertiesBlockArray = it->second.mShaderPropertiesBlockArray;
        it->second.mGPUUniformBuffersContainer.getUniformBuffer(ShaderPropertiesBlockNames::smPropertiesBlockBufferName).setDataArray(shaderPropertiesBlockArray);
    }
}

WeakPtr<GPUTexture> ShaderManager::loadTexture(const GPUTextureData& gpuTextureData)
{
	if (!mTexturesByPath.contains(gpuTextureData.mPath))
	{
        LOG_TRACE()
        PROFILER_CPU()
        WeakPtr<GPUTexture> texture = mTextures.emplace_back(OwnerPtr<GPUTexture>::newObject());
        mTexturesByPath.insert_or_assign(gpuTextureData.mPath, texture);
        texture->init(GET_SYSTEM(GPUInstance).mGPUContext, gpuTextureData, mTextures.size() - 1);
	}

	return mTexturesByPath.at(gpuTextureData.mPath);
}

void ShaderManager::postShaderCreated(WeakPtr<Shader> shader)
{
    loadShaderTextures(shader);
    initShaderInstancePropertiesUniformBuffer(shader);
}

void ShaderManager::loadShaderTextures(WeakPtr<Shader> shader)
{
    u32 id = shader->getID();
    if(!mTextureBindingsByShader.contains(id))
    {
        LOG_TRACE()
        PROFILER_CPU()
        mTextureBindingsByShader.emplace(id, std::unordered_map<HashedString, WeakPtr<GPUTexture>>());

        FOR_MAP(it, shader->getShaderData().mTextureBindings)
        {
            CHECK_MSG(!it->second.mPath.get().empty(), "texture mPath cannot be empty!");
            GPUTextureData gpuTextureData;
            gpuTextureData.mPath = it->second.mPath;

            if(shader->getShaderData().mIsFont)
            {
                CHECK_MSG(!shader->getShaderData().mFontData.mPath.get().empty(), "mShaderData.mFontData.mPath cannot be empty!");
                gpuTextureData.mIsFont = true;
                gpuTextureData.mFontData = shader->getShaderData().mFontData;
            }

            mTextureBindingsByShader.at(id).insert_or_assign(it->first, GET_SYSTEM(ShaderManager).loadTexture(gpuTextureData));
        }
    }
}

const std::unordered_map<HashedString, WeakPtr<GPUTexture>>& ShaderManager::getShaderTextureBindings(u32 id) const
{
    return mTextureBindingsByShader.at(id);
}

WeakPtr<ShaderInstance> ShaderManager::createShaderInstance(WeakPtr<Shader> shader)
{
    LOG_TRACE()
    PROFILER_CPU()
    WeakPtr<ShaderInstance> instance = mShaderInstances.emplace_back(OwnerPtr<ShaderInstance>::newObject());
    instance->mShader = shader;
    instance->mID = mShaderInstances.size() - 1;
        instance->mShaderPropertiesBlockBuffer = shader->getShaderData().mSharedShaderPropertiesBlockBuffer;
    instance->mSlot = requestShaderInstanceSlot(shader);

    return instance;
}
void ShaderManager::freeShaderInstance(WeakPtr<ShaderInstance> shaderInstance)
{
    LOG_TRACE()
    PROFILER_CPU()
    CHECK_MSG(shaderInstance->mShader.isValid(), "Invalid shader!");
    ClassId propertiesBlockClassId = shaderInstance->mShader->getShaderData().mSharedShaderPropertiesBlockClass.getId();

    if(mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shaderInstance->mShader->getShaderData().mAllowInstances)
        {
            mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.freeSlot(shaderInstance->mSlot);
        }
    }  
}

void ShaderManager::initShaderInstancePropertiesUniformBuffer(WeakPtr<Shader> shader)
{
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    u32 shaderID = shader->getID();
    ClassId propertiesBlockClassId = shader->getShaderData().mSharedShaderPropertiesBlockClass.getId();
    
    if(!mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shader->getShaderData().allowInstances())
        {
            u32 propertiesBlockSizeBytes = shader->getShaderData().getSharedShaderPropertiesBlockBufferSize();
            if(propertiesBlockSizeBytes > 0)
            {
                LOG_TRACE()
                PROFILER_CPU()
                mShaderPropertyBlockRenderStates.emplace(propertiesBlockClassId, ShaderPropertyBlockRenderState());

                // u32 size = shader->getShaderData().getMaxInstances();
                mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.init(mInitialInstances);
                mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mShaderPropertiesBlockArray.resize(mInitialInstances * propertiesBlockSizeBytes);

                // Reserve index 0 for default shader instance
                Slot defaultSlot = mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.requestSlot();
                mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mShaderPropertiesBlockArray.copyBufferAt(shader->getShaderData().mSharedShaderPropertiesBlockBuffer.getByteBuffer(), defaultSlot.getSlot() * propertiesBlockSizeBytes);

                const GPUUniformBufferData& propertiesBlockUniformBufferData = shader->getPropertiesBlockUniformBufferData();
                mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.addUniformBuffer(propertiesBlockUniformBufferData, propertiesBlockSizeBytes * mInitialInstances, false);
            }
        }
    }
}

void ShaderManager::setShaderInstanceProperties(WeakPtr<ShaderInstance> shaderInstance)
{
    PROFILER_CPU()

    WeakPtr<Shader> shader = shaderInstance->mShader;
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    u32 shaderID = shader->getID();
    ClassId propertiesBlockClassId = shader->getShaderData().mSharedShaderPropertiesBlockClass.getId();

    if(shader->getShaderData().allowInstances())
    {
        PROFILER_CPU_NAMED(allowInstances)
        if(mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            LOG_TRACE()
            PROFILER_CPU()
            CHECK_MSG(mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(shaderInstance->mSlot), "Invalid slot!");
            u32 propertiesBlockSizeBytes = shader->getShaderData().getSharedShaderPropertiesBlockBufferSize();
            PROFILER_CPU_NAMED(CopyBuffer)
            mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mShaderPropertiesBlockArray.copyBufferAt(shaderInstance->mShaderPropertiesBlockBuffer.getByteBuffer(), shaderInstance->mSlot.getSlot() * propertiesBlockSizeBytes);
        }
    }
}

void ShaderManager::setShaderInstanceDirty(u32 id)
{
    PROFILER_CPU()

    WeakPtr<ShaderInstance> shaderInstance = mShaderInstances.at(id);
    CHECK_MSG(shaderInstance.isValid(), "Invalid shader Instance!");
    WeakPtr<Shader> shader = shaderInstance->mShader;
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    u32 shaderID = shader->getID();
    ClassId propertiesBlockClassId = shader->getShaderData().mSharedShaderPropertiesBlockClass.getId();

    if(shader->getShaderData().allowInstances())
    {
        if(mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            CHECK_MSG(mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(shaderInstance->mSlot), "Invalid slot!");
            mDirtyShaderInstances.insert(id);
        }
    }
}

const GPUUniformBuffer& ShaderManager::getShaderPropertiesGPUUniformBuffer(WeakPtr<Shader> shader) const
{
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    ClassId propertiesBlockClassId = shader->getShaderData().mSharedShaderPropertiesBlockClass.getId();
    CHECK_MSG(mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId), "Shader Property Block not found!");
    return mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(ShaderPropertiesBlockNames::smPropertiesBlockBufferName);
}

Slot ShaderManager::requestShaderInstanceSlot(WeakPtr<Shader> shader)
{
    LOG_TRACE()
    PROFILER_CPU()

    CHECK_MSG(shader.isValid(), "Invalid shader!");
    ClassId propertiesBlockClassId = shader->getShaderData().mSharedShaderPropertiesBlockClass.getId();
    
    Slot slot;
    if(mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {    
        if(shader->getShaderData().mAllowInstances)
        {
            if(mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.isEmpty())
            {
                CHECK_MSG(false, "mShaderPropertyBlockRenderStates propertiesBlockClassId mSlotsManager.isEmpty!");

                u32 propertiesBlockSizeBytes = shader->getShaderData().getSharedShaderPropertiesBlockBufferSize();
                mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.increaseSize(mInitialInstances);
                mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mShaderPropertiesBlockArray.resize(mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.getSize() * propertiesBlockSizeBytes);
                // mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(ShaderPropertiesBlockNames::smPropertiesBlockBufferName).resizeBytes(propertiesBlockSizeBytes * mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.getSize());
            }

            slot = mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.requestSlot();
        }
        else
        {
            // point to default shader instance
            slot.set(0);
        }
    }

    return slot;
}