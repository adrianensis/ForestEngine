#include "Graphics/Shader/ShaderManager.hpp"
#include "GPU/Image/GPUTexture.hpp"
#include "GPU/GPUInstance.hpp"

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

    FOR_LIST(it, mDirtyShaderPropertiesInstances)
    {
        WeakPtr<ShaderPropertiesInstance> instance = mShaderPropertiesInstances.at(*it);
        setShaderPropertiesInstanceProperties(instance);
    }
    mDirtyShaderPropertiesInstances.clear();

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
    initShaderPropertiesInstancePropertiesUniformBuffer(shader);
}

void ShaderManager::loadShaderTextures(WeakPtr<Shader> shader)
{
    u32 id = shader->getID();
    if(!mTextureBindingsByShader.contains(id))
    {
        LOG_TRACE()
        PROFILER_CPU()
        mTextureBindingsByShader.emplace(id, std::unordered_map<HashedString, WeakPtr<GPUTexture>>());

        FOR_MAP(it, shader->getShaderData().mShaderTextureBindings.mTextureBindings)
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

WeakPtr<ShaderPropertiesInstance> ShaderManager::createShaderPropertiesInstance(WeakPtr<Shader> shader)
{
    LOG_TRACE()
    PROFILER_CPU()
    WeakPtr<ShaderPropertiesInstance> instance = mShaderPropertiesInstances.emplace_back(OwnerPtr<ShaderPropertiesInstance>::newObject());
    instance->mShader = shader;
    instance->mID = mShaderPropertiesInstances.size() - 1;
    instance->mShaderPropertiesBlockBuffer = shader->getSharedShaderPropertiesBlockBuffer();
    instance->mSlot = requestShaderPropertiesInstanceSlot(shader);

    return instance;
}
void ShaderManager::freeShaderPropertiesInstance(WeakPtr<ShaderPropertiesInstance> shaderPropertiesInstance)
{
    LOG_TRACE()
    PROFILER_CPU()
    CHECK_MSG(shaderPropertiesInstance->mShader.isValid(), "Invalid shader!");
    ClassId propertiesBlockClassId = shaderPropertiesInstance->mShader->getSharedShaderPropertiesBlockClass().getId();

    if(mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shaderPropertiesInstance->mShader->getShaderData().mAllowInstances)
        {
            mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.freeSlot(shaderPropertiesInstance->mSlot);
        }
    }  
}

void ShaderManager::initShaderPropertiesInstancePropertiesUniformBuffer(WeakPtr<Shader> shader)
{
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    u32 shaderID = shader->getID();
    ClassId propertiesBlockClassId = shader->getSharedShaderPropertiesBlockClass().getId();
    
    if(!mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shader->allowInstances())
        {
            u32 propertiesBlockSizeBytes = shader->getSharedShaderPropertiesBlockBuffer().getByteBuffer().size();
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
                mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mShaderPropertiesBlockArray.copyBufferAt(shader->getSharedShaderPropertiesBlockBuffer().getByteBuffer(), defaultSlot.getSlot() * propertiesBlockSizeBytes);

                const GPUUniformBufferData& propertiesBlockUniformBufferData = shader->getPropertiesBlockUniformBufferData();
                mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.addUniformBuffer(propertiesBlockUniformBufferData, propertiesBlockSizeBytes * mInitialInstances, false);
            }
        }
    }
}

void ShaderManager::setShaderPropertiesInstanceProperties(WeakPtr<ShaderPropertiesInstance> shaderPropertiesInstance)
{
    PROFILER_CPU()

    WeakPtr<Shader> shader = shaderPropertiesInstance->mShader;
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    u32 shaderID = shader->getID();
    ClassId propertiesBlockClassId = shader->getSharedShaderPropertiesBlockClass().getId();

    if(shader->allowInstances())
    {
        PROFILER_CPU_NAMED(allowInstances)
        if(mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            LOG_TRACE()
            PROFILER_CPU()
            CHECK_MSG(mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(shaderPropertiesInstance->mSlot), "Invalid slot!");
            u32 propertiesBlockSizeBytes = shader->getSharedShaderPropertiesBlockBuffer().getByteBuffer().size();
            PROFILER_CPU_NAMED(CopyBuffer)
            mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mShaderPropertiesBlockArray.copyBufferAt(shaderPropertiesInstance->mShaderPropertiesBlockBuffer.getByteBuffer(), shaderPropertiesInstance->mSlot.getSlot() * propertiesBlockSizeBytes);
        }
    }
}

void ShaderManager::setShaderPropertiesInstanceDirty(u32 id)
{
    PROFILER_CPU()

    WeakPtr<ShaderPropertiesInstance> shaderPropertiesInstance = mShaderPropertiesInstances.at(id);
    CHECK_MSG(shaderPropertiesInstance.isValid(), "Invalid shader Instance!");
    WeakPtr<Shader> shader = shaderPropertiesInstance->mShader;
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    u32 shaderID = shader->getID();
    ClassId propertiesBlockClassId = shader->getSharedShaderPropertiesBlockClass().getId();

    if(shader->allowInstances())
    {
        if(mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            CHECK_MSG(mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(shaderPropertiesInstance->mSlot), "Invalid slot!");
            mDirtyShaderPropertiesInstances.insert(id);
        }
    }
}

const GPUUniformBuffer& ShaderManager::getShaderPropertiesGPUUniformBuffer(WeakPtr<Shader> shader) const
{
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    ClassId propertiesBlockClassId = shader->getSharedShaderPropertiesBlockClass().getId();
    CHECK_MSG(mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId), "Shader Property Block not found!");
    return mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(ShaderPropertiesBlockNames::smPropertiesBlockBufferName);
}

Slot ShaderManager::requestShaderPropertiesInstanceSlot(WeakPtr<Shader> shader)
{
    LOG_TRACE()
    PROFILER_CPU()

    CHECK_MSG(shader.isValid(), "Invalid shader!");
    ClassId propertiesBlockClassId = shader->getSharedShaderPropertiesBlockClass().getId();
    
    Slot slot;
    if(mShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {    
        if(shader->getShaderData().mAllowInstances)
        {
            if(mShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.isEmpty())
            {
                CHECK_MSG(false, "mShaderPropertyBlockRenderStates propertiesBlockClassId mSlotsManager.isEmpty!");

                u32 propertiesBlockSizeBytes = shader->getSharedShaderPropertiesBlockBuffer().getByteBuffer().size();
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