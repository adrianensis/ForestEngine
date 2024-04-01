#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPULight.hpp"

void RenderSharedContext::init()
{
    mMaxSharedBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForSharedBuffer(GPUBufferType::UNIFORM);
    mMaxSharedBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForSharedBuffer(GPUBufferType::STORAGE);

    u32 bindingPointGlobalData = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mGlobalData.mType);
    mGPUSharedBuffersContainer.createSharedBuffer(bindingPointGlobalData, GPUBuiltIn::SharedBuffers::mGlobalData, false);
    mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).resize<GPUBuiltIn::SharedBuffers::GPUGlobalData>(1);

    u32 bindingPointLightsData = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mLightsData.mType);
    mGPUSharedBuffersContainer.createSharedBuffer(bindingPointLightsData, GPUBuiltIn::SharedBuffers::mLightsData, false);
    mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mLightsData).resize<GPULightsData>(1);

    mRenderInstancesSlotsManager.init(mMaxInstances);

    mMatrices.resize(mRenderInstancesSlotsManager.getSize());
    u32 bindingPointModelMatrices = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mModelMatrices.mType);
    mGPUSharedBuffersContainer.createSharedBuffer(bindingPointModelMatrices, GPUBuiltIn::SharedBuffers::mModelMatrices, false);
    mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
}

void RenderSharedContext::update()
{
	PROFILER_CPU()
	mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).setDataArray(mMatrices);

    FOR_MAP(it, mMaterialInstancesDataMap)
    {
        PoolHandler<Material> material = it->second.mMaterial;
        const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getPropertiesBlockSharedBufferData();

        ByteBuffer& materialPropertiesBlockArray = it->second.mMaterialPropertiesBlockArray;
	    it->second.mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData).setDataArray(materialPropertiesBlockArray);
    }
}

u32 RenderSharedContext::requestSharedBufferBindingPoint(GPUBufferType gpuSharedBufferType)
{
    u32 bindingPoint = 0;
    switch (gpuSharedBufferType)
    {
    case GPUBufferType::UNIFORM:
        {
            bindingPoint = mBindingPointsIndexUniform;
            mBindingPointsIndexUniform++;
            CHECK_MSG((i32)mBindingPointsIndexUniform <= mMaxSharedBufferBindingPointsUniform, "Max Uniform Binding Points reached!");
        }
        break;
    case GPUBufferType::STORAGE:
        {
            bindingPoint = mBindingPointsIndexStorage;
            mBindingPointsIndexStorage++;
            CHECK_MSG((i32)mBindingPointsIndexStorage <= mMaxSharedBufferBindingPointsStorage, "Max Storage Binding Points reached!");
        }
        break;
    default:
        CHECK_MSG(false, "Ilegal GPUBufferType!");
        break;
    }

    return bindingPoint;
}

void RenderSharedContext::setInstanceMatrix(const Slot& slot, const Matrix4& matrix)
{
    CHECK_MSG(slot.getIsValid(), "Invalid slot!");
    mMatrices.at(slot.getSlot()) = matrix;
}

void RenderSharedContext::initMaterialInstancePropertiesSharedBuffer(const PoolHandler<Material>& material)
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(!mMaterialInstancesDataMap.contains(materialID), "Material already registered!");

    mMaterialInstancesDataMap.emplace(materialID, SharedContextMaterialInstancedData());

    u32 size = material->getMaterialData().mAllowInstances ? material->getMaterialData().mMaxInstances : 1;
    u32 propertiesBlockSizeBytes = material->getMaterialData().mSharedMaterialPropertiesBlockBuffer.getByteBuffer().size();
    mMaterialInstancesDataMap.at(materialID).mMaterial = material;
    mMaterialInstancesDataMap.at(materialID).mSlotsManager.init(size);
    mMaterialInstancesDataMap.at(materialID).mMaterialPropertiesBlockArray.resize(size * propertiesBlockSizeBytes);

    // Reserve index 0 for default material instance
    Slot defaultSlot = mMaterialInstancesDataMap.at(materialID).mSlotsManager.requestSlot();
    mMaterialInstancesDataMap.at(materialID).mMaterialPropertiesBlockArray.setAt(material->getMaterialData().mSharedMaterialPropertiesBlockBuffer.getByteBuffer(), defaultSlot.getSlot() * propertiesBlockSizeBytes);

    const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getPropertiesBlockSharedBufferData();
    u32 bindingPointMaterialPropertiesBlock = requestSharedBufferBindingPoint(propertiesBlockSharedBufferData.mType);
    mMaterialInstancesDataMap.at(materialID).mGPUSharedBuffersContainer.createSharedBuffer(bindingPointMaterialPropertiesBlock, propertiesBlockSharedBufferData, false);
    mMaterialInstancesDataMap.at(materialID).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData).resizeBytes(propertiesBlockSizeBytes * size);
}

void RenderSharedContext::setMaterialInstanceProperties(const Slot& slot, const MaterialInstance& materialInstance)
{
    PoolHandler<Material> material = materialInstance.mMaterial;
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(mMaterialInstancesDataMap.contains(materialID), "Invalid material!");

    if(material->getMaterialData().mAllowInstances)
    {
        u32 propertiesBlockSizeBytes = material->getMaterialData().mSharedMaterialPropertiesBlockBuffer.getByteBuffer().size();
        mMaterialInstancesDataMap.at(materialID).mMaterialPropertiesBlockArray.setAt(materialInstance.mMaterialPropertiesBlockBuffer.getByteBuffer(), slot.getSlot() * propertiesBlockSizeBytes);
    }
}

const GPUSharedBuffer& RenderSharedContext::getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getPropertiesBlockSharedBufferData();
    return mMaterialInstancesDataMap.at(material->getID()).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData);
}

Slot RenderSharedContext::requestMaterialInstanceSlot(const PoolHandler<Material>& material)
{
    CHECK_MSG(material.isValid(), "Invalid material!");

    Slot slot;
    if(material->getMaterialData().mAllowInstances)
    {
        slot = mMaterialInstancesDataMap.at(material->getID()).mSlotsManager.requestSlot();
    }
    else
    {
        slot.set(0);
    }

    return slot;
}

void RenderSharedContext::freeMaterialInstanceSlot(const PoolHandler<Material>& material, Slot& slot)
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    if(material->getMaterialData().mAllowInstances)
    {
        mMaterialInstancesDataMap.at(material->getID()).mSlotsManager.freeSlot(slot);
    }
}

void RenderSharedContext::terminate()
{
    mGPUSharedBuffersContainer.terminate();

    FOR_MAP(it, mMaterialInstancesDataMap)
    {
        it->second.mGPUSharedBuffersContainer.terminate();
    }

    mMaterialInstancesDataMap.clear();
    mRenderInstancesSlotsManager.reset();
}