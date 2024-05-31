#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/RenderState.hpp"

void MaterialManager::init()
{
	LOG_TRACE()
}

void MaterialManager::terminate()
{
    FOR_MAP(it, mMaterialRenderStates)
    {
        it->second.mGPUSharedBuffersContainer.terminate();
    }

    mMaterialRenderStates.clear();
}

void MaterialManager::update()
{
    FOR_MAP(it, mMaterialRenderStates)
    {
        PoolHandler<Material> material = it->second.mMaterial;
        const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;

        ByteBuffer& materialPropertiesBlockArray = it->second.mMaterialPropertiesBlockArray;
	    it->second.mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData).setDataArray(materialPropertiesBlockArray);
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
    CHECK_MSG(!mMaterialRenderStates.contains(materialID), "Material already registered!");

    mMaterialRenderStates.emplace(materialID, MaterialRenderState());

    u32 size = material->getMaterialData().getMaxInstances();
    u32 propertiesBlockSizeBytes = material->getMaterialData().getSharedMaterialPropertiesBlockBufferSize();
    mMaterialRenderStates.at(materialID).mMaterial = material;
    mMaterialRenderStates.at(materialID).mSlotsManager.init(size);
    mMaterialRenderStates.at(materialID).mMaterialPropertiesBlockArray.resize(size * propertiesBlockSizeBytes);

    // Reserve index 0 for default material instance
    Slot defaultSlot = mMaterialRenderStates.at(materialID).mSlotsManager.requestSlot();
    mMaterialRenderStates.at(materialID).mMaterialPropertiesBlockArray.setAt(material->getMaterialData().mSharedMaterialPropertiesBlockBuffer.getByteBuffer(), defaultSlot.getSlot() * propertiesBlockSizeBytes);

    const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;
    mMaterialRenderStates.at(materialID).mGPUSharedBuffersContainer.addSharedBuffer(propertiesBlockSharedBufferData, false);
    mMaterialRenderStates.at(materialID).mGPUSharedBuffersContainer.create();
    mMaterialRenderStates.at(materialID).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData).resizeBytes(propertiesBlockSizeBytes * size);
}

void MaterialManager::setMaterialInstanceProperties(const Slot& slot, const MaterialInstance& materialInstance)
{
    PoolHandler<Material> material = materialInstance.mMaterial;
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(mMaterialRenderStates.contains(materialID), "Invalid material!");

    CHECK_MSG(mMaterialRenderStates.at(materialID).mSlotsManager.checkSlot(slot), "Invalid slot!");

    if(material->getMaterialData().allowInstances())
    {
        u32 propertiesBlockSizeBytes = material->getMaterialData().getSharedMaterialPropertiesBlockBufferSize();
        mMaterialRenderStates.at(materialID).mMaterialPropertiesBlockArray.setAt(materialInstance.mMaterialPropertiesBlockBuffer.getByteBuffer(), slot.getSlot() * propertiesBlockSizeBytes);
    }
}

const GPUSharedBuffer& MaterialManager::getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;
    return mMaterialRenderStates.at(material->getID()).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData);
}

Slot MaterialManager::requestMaterialInstanceSlot(const PoolHandler<Material>& material)
{
    CHECK_MSG(material.isValid(), "Invalid material!");

    Slot slot;
    if(material->getMaterialData().mAllowInstances)
    {
        slot = mMaterialRenderStates.at(material->getID()).mSlotsManager.requestSlot();
    }
    else
    {
        // point to default material instance
        slot.set(0);
    }

    return slot;
}

void MaterialManager::freeMaterialInstanceSlot(const PoolHandler<Material>& material, Slot& slot)
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    if(material->getMaterialData().mAllowInstances)
    {
        mMaterialRenderStates.at(material->getID()).mSlotsManager.freeSlot(slot);
    }
}
