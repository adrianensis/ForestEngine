#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"

void MaterialManager::init()
{
	LOG_TRACE()
}

void MaterialManager::terminate()
{
    FOR_MAP(it, mMaterialPropertyBlockRenderStates)
    {
        it->second.mGPUSharedBuffersContainer.terminate();
    }

    mMaterialPropertyBlockRenderStates.clear();
}

void MaterialManager::update()
{
    FOR_MAP(it, mMaterialToPropertyBlock)
    {
        PoolHandler<Material> material = mMaterials.getHandler(it->first);
        const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;
        ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();
        if(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId))
        {
            ByteBuffer& materialPropertiesBlockArray = mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mMaterialPropertiesBlockArray;
            mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData).setDataArray(materialPropertiesBlockArray);
        }
    }
}

PoolHandler<Texture> MaterialManager::loadTexture(const TextureData& textureData)
{
	if (!mTexturesByPath.contains(textureData.mPath))
	{
        PoolHandler<Texture> handler = mTextures.allocate();
        mTexturesByPath.insert_or_assign(textureData.mPath, handler);
        Texture& texure = mTextures.get(handler);
        texure.init(textureData, handler.getIndex());
	}

	return mTexturesByPath.at(textureData.mPath);
}

void MaterialManager::unloadTexture(PoolHandler<Texture>& texture)
{
    mTextures.free(texture);
}

void MaterialManager::postMaterialCreated(const PoolHandler<Material>& handler)
{
    initMaterialInstancePropertiesSharedBuffer(handler);
}

MaterialInstance MaterialManager::createMaterialInstance(const PoolHandler<Material>& handler)
{
    Material& material = mMaterials.get(handler);
    MaterialInstance instance;
    instance.mMaterial = handler;
    instance.mMaterialPropertiesBlockBuffer = material.getMaterialData().mSharedMaterialPropertiesBlockBuffer;
    return instance;
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

void MaterialManager::initMaterialInstancePropertiesSharedBuffer(const PoolHandler<Material>& material)
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
                mMaterialPropertyBlockRenderStates.emplace(propertiesBlockClassId, MaterialPropertyBlockRenderState());

                u32 size = 10000;
                // u32 size = material->getMaterialData().getMaxInstances();
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.init(size);
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mMaterialPropertiesBlockArray.resize(size * propertiesBlockSizeBytes);

                // Reserve index 0 for default material instance
                Slot defaultSlot = mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.requestSlot();
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mMaterialPropertiesBlockArray.copyBufferAt(material->getMaterialData().mSharedMaterialPropertiesBlockBuffer.getByteBuffer(), defaultSlot.getSlot() * propertiesBlockSizeBytes);

                const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUSharedBuffersContainer.addSharedBuffer(propertiesBlockSharedBufferData, false);
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUSharedBuffersContainer.create();
                mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData).resizeBytes(propertiesBlockSizeBytes * size);
            }
        }
    }
}

void MaterialManager::setMaterialInstanceProperties(const Slot& slot, const MaterialInstance& materialInstance)
{
    PoolHandler<Material> material = materialInstance.mMaterial;
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(mMaterialToPropertyBlock.contains(materialID), "Invalid material!");
    ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();

    if(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        CHECK_MSG(mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.checkSlot(slot), "Invalid slot!");
        if(material->getMaterialData().allowInstances())
        {
            u32 propertiesBlockSizeBytes = material->getMaterialData().getSharedMaterialPropertiesBlockBufferSize();
            mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mMaterialPropertiesBlockArray.copyBufferAt(materialInstance.mMaterialPropertiesBlockBuffer.getByteBuffer(), slot.getSlot() * propertiesBlockSizeBytes);
        }
    }
}

const GPUSharedBuffer& MaterialManager::getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;
    ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();
    CHECK_MSG(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId), "Material Property Block not found!");
    return mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData);
}

Slot MaterialManager::requestMaterialInstanceSlot(const PoolHandler<Material>& material)
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();
    
    Slot slot;
    if(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {    
        if(material->getMaterialData().mAllowInstances)
        {
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

void MaterialManager::freeMaterialInstanceSlot(const PoolHandler<Material>& material, const Slot& slot)
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    ClassId propertiesBlockClassId = material->getMaterialData().mSharedMaterialPropertiesBlockClass.getId();

    if(mMaterialPropertyBlockRenderStates.contains(propertiesBlockClassId))
    {
        if(material->getMaterialData().mAllowInstances)
        {
            mMaterialPropertyBlockRenderStates.at(propertiesBlockClassId).mSlotsManager.freeSlot(slot);
        }
    }    
}
