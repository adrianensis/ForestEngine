#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/GPU/GPUTexture.hpp"
#include "Graphics/GPU/GPUInstance.hpp"

void MaterialInstance::setDirty()
{
    PROFILER_CPU();
    GET_SYSTEM(MaterialManager).setMaterialInstanceDirty(mID);
}

void MaterialManager::init()
{
	LOG_TRACE()
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUBuiltIn::UniformBuffers::mTextures, false);
}

void MaterialManager::terminate()
{
    FOR_MAP(it, mMaterialPropertyBlockRenderStates)
    {
        it->second.mGPUUniformBuffersContainer.terminate();
    }

    mMaterialPropertyBlockRenderStates.clear();
}

void MaterialManager::update()
{
    PROFILER_CPU();

    FOR_LIST(it, mDirtyMaterialInstances)
    {
        PoolHandler<MaterialInstance> instance = mMaterialInstances.getHandler(*it);
        setMaterialInstanceProperties(instance);
    }
    mDirtyMaterialInstances.clear();

    FOR_MAP(it, mMaterialToPropertyBlock)
    {
        PoolHandler<Material> material = mMaterials.getHandler(it->first);
        ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();
        if(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            ByteBuffer& materialPropertiesBlockArray = mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mMaterialPropertiesBlockArray;
            mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(ShaderPropertiesBlockNames::smPropertiesBlockBufferName).setDataArray(materialPropertiesBlockArray);
        }
    }

    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mTextures).setDataArray<TextureHandle>(mTextureHandles);
}

PoolHandler<GPUTexture> MaterialManager::loadTexture(const GPUTextureData& gpuTextureData)
{
	if (!mTexturesByPath.contains(gpuTextureData.mPath))
	{
        LOG_TRACE()
        PROFILER_CPU()
        PoolHandler<GPUTexture> handler = mTextures.allocate();
        mTexturesByPath.insert_or_assign(gpuTextureData.mPath, handler);
        GPUTexture& texture = mTextures.get(handler);
        texture.init(gpuTextureData, handler.getIndex());

        u32 size = mTextures.getSize();
        // NOTE: We reserve position 0 to represent NULL
        u32 paddedSize = size + 1;
        mTextureHandles.resize(paddedSize);
        GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mTextures).resize<TextureHandle>(paddedSize);

        mTextureHandles[texture.getID() + 1] = texture.getGPUTextureHandle();
	}

	return mTexturesByPath.at(gpuTextureData.mPath);
}

void MaterialManager::unloadTexture(PoolHandler<GPUTexture>& texture)
{
    mTextures.free(texture);
}

void MaterialManager::postMaterialCreated(const PoolHandler<Material>& handler)
{
    loadMaterialTextures(handler);
    initMaterialInstancePropertiesUniformBuffer(handler);
}

void MaterialManager::loadMaterialTextures(const PoolHandler<Material>& handler)
{
    u32 id = handler->getID();
    if(!mTextureBindingsByMaterial.contains(id))
    {
        LOG_TRACE()
        PROFILER_CPU()
        mTextureBindingsByMaterial.emplace(id, std::unordered_map<HashedString, PoolHandler<GPUTexture>>());

        FOR_MAP(it, handler->getMaterialData().mTextureBindings)
        {
            CHECK_MSG(!it->second.mPath.get().empty(), "texture mPath cannot be empty!");
            GPUTextureData gpuTextureData;
            gpuTextureData.mPath = it->second.mPath;
            gpuTextureData.mStage = it->second.mStage;

            if(handler->getMaterialData().mIsFont)
            {
                CHECK_MSG(!handler->getMaterialData().mFontData.mPath.get().empty(), "mMaterialData.mFontData.mPath cannot be empty!");
                gpuTextureData.mIsFont = true;
                gpuTextureData.mFontData = handler->getMaterialData().mFontData;
            }

            mTextureBindingsByMaterial.at(id).insert_or_assign(it->first, GET_SYSTEM(MaterialManager).loadTexture(gpuTextureData));
        }
    }
}

const std::unordered_map<HashedString, PoolHandler<GPUTexture>>& MaterialManager::getMaterialTextureBindings(const PoolHandler<Material>& handler) const
{
    return mTextureBindingsByMaterial.at(handler->getID());
}

PoolHandler<MaterialInstance> MaterialManager::createMaterialInstance(const PoolHandler<Material>& handler)
{
    LOG_TRACE()
    PROFILER_CPU()
    PoolHandler<MaterialInstance> instance = mMaterialInstances.allocate();
    instance->mMaterial = handler;
    instance->mID = instance.getIndex();
    instance->mMaterialPropertiesBlockBuffer = handler->getMaterialData().mSharedMaterialPropertiesBlockBuffer;
    instance->mSlot = requestMaterialInstanceSlot(handler);

    return instance;
}
void MaterialManager::freeMaterialInstance(PoolHandler<MaterialInstance> materialInstance)
{
    LOG_TRACE()
    PROFILER_CPU()
    CHECK_MSG(materialInstance->mMaterial.isValid(), "Invalid material!");
    ClassId propertiesBlockClassId = materialInstance->mMaterial->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();

    if(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(materialInstance->mMaterial->getMaterialData().mAllowInstances)
        {
            mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.freeSlot(materialInstance->mSlot);
        }
    }  
}

void MaterialManager::removeMaterial(PoolHandler<Material>& material)
{
    mMaterials.free(material);
}

const Material& MaterialManager::getMaterial(const PoolHandler<Material>& handler) const
{
    CHECK_MSG(handler.isValid(), "Invalid handler!");
    return mMaterials.get(handler);
}

PoolHandler<Material> MaterialManager::getMaterialHandler(u32 id) const
{
    PoolHandler<Material> handler = mMaterials.getHandler(id);
    CHECK_MSG(handler.isValid(), "Invalid handler!");
    return handler;
}

const Material& MaterialManager::getMaterial(u32 id) const
{
    return getMaterialHandler(id).get();
}

void MaterialManager::initMaterialInstancePropertiesUniformBuffer(const PoolHandler<Material>& material)
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(!mMaterialToPropertyBlock.contains(materialID), "Material already registered!");
    
    ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();
    mMaterialToPropertyBlock.emplace(materialID, propertiesBlockClassId);
    
    if(!mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(material->getMaterialData().allowInstances())
        {
            mMaterialToPropertyBlock.emplace(materialID, propertiesBlockClassId);

            u32 propertiesBlockSizeBytes = material->getMaterialData().getSharedMaterialPropertiesBlockBufferSize();
            if(propertiesBlockSizeBytes > 0)
            {
                LOG_TRACE()
                PROFILER_CPU()
                mMaterialPropertyBlockRenderStates.emplace(propertiesBlockClassId, MaterialPropertyBlockRenderState());

                // u32 size = material->getMaterialData().getMaxInstances();
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.init(mInitialInstances);
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mMaterialPropertiesBlockArray.resize(mInitialInstances * propertiesBlockSizeBytes);

                // Reserve index 0 for default material instance
                Slot defaultSlot = mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.requestSlot();
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mMaterialPropertiesBlockArray.copyBufferAt(material->getMaterialData().mSharedMaterialPropertiesBlockBuffer.getByteBuffer(), defaultSlot.getSlot() * propertiesBlockSizeBytes);

                const GPUUniformBufferData& propertiesBlockUniformBufferData = material->getShader()->getShaderData().mPropertiesBlockUniformBufferData;
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.addUniformBuffer(propertiesBlockUniformBufferData, false);
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.create();
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(ShaderPropertiesBlockNames::smPropertiesBlockBufferName).resizeBytes(propertiesBlockSizeBytes * mInitialInstances);
            }
        }
    }
}

void MaterialManager::setMaterialInstanceProperties(PoolHandler<MaterialInstance> materialInstance)
{
    PROFILER_CPU()

    PoolHandler<Material> material = materialInstance->mMaterial;
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(mMaterialToPropertyBlock.contains(materialID), "Invalid material!");
    ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();

    if(material->getMaterialData().allowInstances())
    {
        PROFILER_BLOCK_CPU(allowInstances)
        if(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            LOG_TRACE()
            PROFILER_CPU()
            CHECK_MSG(mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(materialInstance->mSlot), "Invalid slot!");
            u32 propertiesBlockSizeBytes = material->getMaterialData().getSharedMaterialPropertiesBlockBufferSize();
            PROFILER_BLOCK_CPU(CopyBuffer)
            mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mMaterialPropertiesBlockArray.copyBufferAt(materialInstance->mMaterialPropertiesBlockBuffer.getByteBuffer(), materialInstance->mSlot.getSlot() * propertiesBlockSizeBytes);
            PROFILER_END_BLOCK()
        }
        PROFILER_END_BLOCK()
    }
}

void MaterialManager::setMaterialInstanceDirty(u32 id)
{
    PROFILER_CPU()

    PoolHandler<MaterialInstance> materialInstance = mMaterialInstances.getHandler(id);
    CHECK_MSG(materialInstance.isValid(), "Invalid material Instance!");
    PoolHandler<Material> material = materialInstance->mMaterial;
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(mMaterialToPropertyBlock.contains(materialID), "Invalid material!");
    ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();

    if(material->getMaterialData().allowInstances())
    {
        if(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            CHECK_MSG(mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(materialInstance->mSlot), "Invalid slot!");
            mDirtyMaterialInstances.insert(id);
        }
    }
}

const GPUUniformBuffer& MaterialManager::getMaterialPropertiesGPUUniformBuffer(const PoolHandler<Material>& material) const
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();
    CHECK_MSG(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId), "Material Property Block not found!");
    return mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(ShaderPropertiesBlockNames::smPropertiesBlockBufferName);
}

Slot MaterialManager::requestMaterialInstanceSlot(const PoolHandler<Material>& material)
{
    LOG_TRACE()
    PROFILER_CPU()

    CHECK_MSG(material.isValid(), "Invalid material!");
    ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();
    
    Slot slot;
    if(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {    
        if(material->getMaterialData().mAllowInstances)
        {
            if(mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.isEmpty())
            {
                u32 propertiesBlockSizeBytes = material->getMaterialData().getSharedMaterialPropertiesBlockBufferSize();
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.increaseSize(mInitialInstances);
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mMaterialPropertiesBlockArray.resize(mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.getSize() * propertiesBlockSizeBytes);
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUUniformBuffersContainer.getUniformBuffer(ShaderPropertiesBlockNames::smPropertiesBlockBufferName).resizeBytes(propertiesBlockSizeBytes * mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.getSize());
            }

            slot = mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.requestSlot();
        }
        else
        {
            // point to default material instance
            slot.set(0);
        }
    }

    return slot;
}