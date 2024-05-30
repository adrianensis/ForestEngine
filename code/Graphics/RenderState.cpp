#include "Graphics/RenderState.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUState.hpp"
#include "Graphics/Light/Light.hpp"

void RenderState::init()
{
    GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().addSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData, false);

    GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().addSharedBuffer(LightBuiltIn::mLightsBufferData, false);

    GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().addSharedBuffer(LightBuiltIn::mShadowMappingBufferData, false);

    mRenderInstancesSlotsManager.init(mMaxInstances);

    mMatrices.resize(mRenderInstancesSlotsManager.getSize());
    GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().addSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices, false);

    GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().create();
    GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).resize<GPUBuiltIn::SharedBuffers::GPUGlobalData>(1);
    GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mLightsBufferData).resize<LightBuiltIn::LightsData>(1);
    GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData).resize<LightBuiltIn::ShadowMappingData>(1);
    GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
}

void RenderState::update()
{
	PROFILER_CPU()
	GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).setDataArray(mMatrices);

    FOR_MAP(it, mMaterialInstancesDataMap)
    {
        PoolHandler<Material> material = it->second.mMaterial;
        const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;

        ByteBuffer& materialPropertiesBlockArray = it->second.mMaterialPropertiesBlockArray;
	    it->second.mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData).setDataArray(materialPropertiesBlockArray);
    }
}

void RenderState::setInstanceMatrix(const Slot& slot, const Matrix4& matrix)
{
    CHECK_MSG(mRenderInstancesSlotsManager.checkSlot(slot), "Invalid slot!");
    mMatrices.at(slot.getSlot()) = matrix;
}

Slot RenderState::requestRenderInstanceSlot()
{
    Slot slot = mRenderInstancesSlotsManager.requestSlot();
    return slot;
}

void RenderState::freeRenderInstanceSlot(Slot& slot)
{
    mRenderInstancesSlotsManager.freeSlot(slot);
}

void RenderState::initMaterialInstancePropertiesSharedBuffer(const PoolHandler<Material>& material)
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(!mMaterialInstancesDataMap.contains(materialID), "Material already registered!");

    mMaterialInstancesDataMap.emplace(materialID, SharedContextMaterialInstancedData());

    u32 size = material->getMaterialData().getMaxInstances();
    u32 propertiesBlockSizeBytes = material->getMaterialData().getSharedMaterialPropertiesBlockBufferSize();
    mMaterialInstancesDataMap.at(materialID).mMaterial = material;
    mMaterialInstancesDataMap.at(materialID).mSlotsManager.init(size);
    mMaterialInstancesDataMap.at(materialID).mMaterialPropertiesBlockArray.resize(size * propertiesBlockSizeBytes);

    // Reserve index 0 for default material instance
    Slot defaultSlot = mMaterialInstancesDataMap.at(materialID).mSlotsManager.requestSlot();
    mMaterialInstancesDataMap.at(materialID).mMaterialPropertiesBlockArray.setAt(material->getMaterialData().mSharedMaterialPropertiesBlockBuffer.getByteBuffer(), defaultSlot.getSlot() * propertiesBlockSizeBytes);

    const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;
    mMaterialInstancesDataMap.at(materialID).mGPUSharedBuffersContainer.addSharedBuffer(propertiesBlockSharedBufferData, false);
    mMaterialInstancesDataMap.at(materialID).mGPUSharedBuffersContainer.create();
    mMaterialInstancesDataMap.at(materialID).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData).resizeBytes(propertiesBlockSizeBytes * size);
}

void RenderState::setMaterialInstanceProperties(const Slot& slot, const MaterialInstance& materialInstance)
{
    PoolHandler<Material> material = materialInstance.mMaterial;
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(mMaterialInstancesDataMap.contains(materialID), "Invalid material!");

    CHECK_MSG(mMaterialInstancesDataMap.at(materialID).mSlotsManager.checkSlot(slot), "Invalid slot!");

    if(material->getMaterialData().allowInstances())
    {
        u32 propertiesBlockSizeBytes = material->getMaterialData().getSharedMaterialPropertiesBlockBufferSize();
        mMaterialInstancesDataMap.at(materialID).mMaterialPropertiesBlockArray.setAt(materialInstance.mMaterialPropertiesBlockBuffer.getByteBuffer(), slot.getSlot() * propertiesBlockSizeBytes);
    }
}

const GPUSharedBuffer& RenderState::getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;
    return mMaterialInstancesDataMap.at(material->getID()).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData);
}

Slot RenderState::requestMaterialInstanceSlot(const PoolHandler<Material>& material)
{
    CHECK_MSG(material.isValid(), "Invalid material!");

    Slot slot;
    if(material->getMaterialData().mAllowInstances)
    {
        slot = mMaterialInstancesDataMap.at(material->getID()).mSlotsManager.requestSlot();
    }
    else
    {
        // point to default material instance
        slot.set(0);
    }

    return slot;
}

void RenderState::freeMaterialInstanceSlot(const PoolHandler<Material>& material, Slot& slot)
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    if(material->getMaterialData().mAllowInstances)
    {
        mMaterialInstancesDataMap.at(material->getID()).mSlotsManager.freeSlot(slot);
    }
}

void RenderState::terminate()
{
    FOR_MAP(it, mMaterialInstancesDataMap)
    {
        it->second.mGPUSharedBuffersContainer.terminate();
    }

    mMaterialInstancesDataMap.clear();
    mRenderInstancesSlotsManager.reset();
}