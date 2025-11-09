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
        Core::WeakPtr<GPUShaderPropertiesInstance> instance = mGPUShaderPropertiesInstances.at(*it);
        setGPUShaderPropertiesInstanceProperties(instance);
    }
    mDirtyGPUShaderPropertiesInstances.clear();

    FOR_MAP(it, mGPUShaderPropertyBlockRenderStates)
    {
        Core::ByteBuffer& shaderPropertiesBlockArray = it->second.mGPUShaderPropertiesBlockArray;
        it->second.mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderPropertiesBlockNames::smPropertiesBlockBufferName).setDataArray(shaderPropertiesBlockArray);
    }
}

Core::WeakPtr<GPUTexture> GPUShaderManager::loadTexture(const GPUTextureData& gpuTextureData)
{
	if (!mTexturesByPath.contains(gpuTextureData.mPath))
	{
        PROFILER_CPU()
        Core::WeakPtr<GPUTexture> texture = mTextures.emplace_back(Core::OwnerPtr<GPUTexture>::newObject());
        mTexturesByPath.insert_or_assign(gpuTextureData.mPath, texture);
        texture->init(GPUInstance::getInstance().mGPUContext, gpuTextureData, mTextures.size() - 1);
	}

	return mTexturesByPath.at(gpuTextureData.mPath);
}

void GPUShaderManager::postGPUShaderCreated(Core::WeakPtr<GPUShader> shader)
{
    loadGPUShaderTextures(shader);
    initGPUShaderPropertiesInstancePropertiesUniformBuffer(shader);
}

void GPUShaderManager::loadGPUShaderTextures(Core::WeakPtr<GPUShader> shader)
{
    Core::u32 id = shader->getID();
    if(!mTextureBindingsByShader.contains(id))
    {
        PROFILER_CPU()
        mTextureBindingsByShader.emplace(id, std::unordered_map<Core::HashedString, Core::WeakPtr<GPUTexture>>());

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

const std::unordered_map<Core::HashedString, Core::WeakPtr<GPUTexture>>& GPUShaderManager::getGPUShaderTextureBindings(Core::u32 id) const
{
    return mTextureBindingsByShader.at(id);
}

Core::WeakPtr<GPUShaderPropertiesInstance> GPUShaderManager::createGPUShaderPropertiesInstance(Core::WeakPtr<GPUShader> shader)
{
    PROFILER_CPU()
    Core::WeakPtr<GPUShaderPropertiesInstance> instance = mGPUShaderPropertiesInstances.emplace_back(Core::OwnerPtr<GPUShaderPropertiesInstance>::newObject());
    instance->mShader = shader;
    instance->mID = mGPUShaderPropertiesInstances.size() - 1;
    instance->mGPUShaderPropertiesBlockBuffer = shader->getSharedGPUShaderPropertiesBlockBuffer();
    instance->mSlot = requestGPUShaderPropertiesInstanceSlot(shader);

    return instance;
}
void GPUShaderManager::freeGPUShaderPropertiesInstance(Core::WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance)
{
    PROFILER_CPU()
    CHECK_MSG(shaderPropertiesInstance->mShader.isValid(), "Invalid shader!");
    Core::ClassId propertiesBlockClassId = shaderPropertiesInstance->mShader->getSharedGPUShaderPropertiesBlockClass().getId();

    if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shaderPropertiesInstance->mShader->getGPUShaderData().mAllowInstances)
        {
            mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.freeSlot(shaderPropertiesInstance->mSlot);
        }
    }  
}

void GPUShaderManager::initGPUShaderPropertiesInstancePropertiesUniformBuffer(Core::WeakPtr<GPUShader> shader)
{
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    Core::u32 shaderID = shader->getID();
    Core::ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();
    
    if(!mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(shader->allowInstances())
        {
            Core::u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
            if(propertiesBlockSizeBytes > 0)
            {
                PROFILER_CPU()
                mGPUShaderPropertyBlockRenderStates.emplace(propertiesBlockClassId, GPUShaderPropertyBlockRenderState());

                // Core::u32 size = shader->getGPUShaderData().getMaxInstances();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.init(mInitialInstances);
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.resize(mInitialInstances * propertiesBlockSizeBytes);

                // Reserve index 0 for default shader instance
                Core::Slot defaultSlot = mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.requestSlot();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.copyBufferAt(shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer(), defaultSlot.getSlot() * propertiesBlockSizeBytes);

                const GPUUniformBufferData& propertiesBlockUniformBufferData = shader->getPropertiesBlockUniformBufferData();
                mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.addUniformBuffer(propertiesBlockUniformBufferData, propertiesBlockSizeBytes * mInitialInstances, false);
            }
        }
    }
}

void GPUShaderManager::setGPUShaderPropertiesInstanceProperties(Core::WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance)
{
    PROFILER_CPU()

    Core::WeakPtr<GPUShader> shader = shaderPropertiesInstance->mShader;
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    Core::u32 shaderID = shader->getID();
    Core::ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();

    if(shader->allowInstances())
    {
        PROFILER_CPU_NAMED(allowInstances)
        if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            PROFILER_CPU()
            CHECK_MSG(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(shaderPropertiesInstance->mSlot), "Invalid slot!");
            Core::u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
            PROFILER_CPU_NAMED(CopyBuffer)
            mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUShaderPropertiesBlockArray.copyBufferAt(shaderPropertiesInstance->mGPUShaderPropertiesBlockBuffer.getByteBuffer(), shaderPropertiesInstance->mSlot.getSlot() * propertiesBlockSizeBytes);
        }
    }
}

void GPUShaderManager::setGPUShaderPropertiesInstanceDirty(Core::u32 id)
{
    PROFILER_CPU()

    Core::WeakPtr<GPUShaderPropertiesInstance> shaderPropertiesInstance = mGPUShaderPropertiesInstances.at(id);
    CHECK_MSG(shaderPropertiesInstance.isValid(), "Invalid shader Instance!");
    Core::WeakPtr<GPUShader> shader = shaderPropertiesInstance->mShader;
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    Core::u32 shaderID = shader->getID();
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

const GPUUniformBuffer& GPUShaderManager::getGPUShaderPropertiesGPUUniformBuffer(Core::WeakPtr<GPUShader> shader) const
{
    CHECK_MSG(shader.isValid(), "Invalid shader!");
    Core::ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();
    CHECK_MSG(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId), "GPUShader Property Block not found!");
    return mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderPropertiesBlockNames::smPropertiesBlockBufferName);
}

Core::Slot GPUShaderManager::requestGPUShaderPropertiesInstanceSlot(Core::WeakPtr<GPUShader> shader)
{
    PROFILER_CPU()

    CHECK_MSG(shader.isValid(), "Invalid shader!");
    Core::ClassId propertiesBlockClassId = shader->getSharedGPUShaderPropertiesBlockClass().getId();
    
    Core::Slot slot;
    if(mGPUShaderPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {    
        if(shader->getGPUShaderData().mAllowInstances)
        {
            if(mGPUShaderPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.isEmpty())
            {
                CHECK_MSG(false, "mGPUShaderPropertyBlockRenderStates propertiesBlockClassId mSlotsManager.isEmpty!");

                Core::u32 propertiesBlockSizeBytes = shader->getSharedGPUShaderPropertiesBlockBuffer().getByteBuffer().size();
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