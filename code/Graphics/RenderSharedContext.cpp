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
        const GPUSharedBufferData& instancedPropertiesSharedBufferData = material->getInstancedPropertiesSharedBufferData();

        ByteBuffer& materialInstancedPropertiesArray = it->second.mMaterialInstancedPropertiesArray;
	    it->second.mGPUSharedBuffersContainer.getSharedBuffer(instancedPropertiesSharedBufferData).setDataArray(materialInstancedPropertiesArray);
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
    u32 instancedPropertiesSizeBytes = material->getMaterialData().mSharedMaterialInstancedPropertiesBuffer.getByteBuffer().size();
    mMaterialInstancesDataMap.at(materialID).mMaterial = material;
    mMaterialInstancesDataMap.at(materialID).mSlotsManager.init(size);
    mMaterialInstancesDataMap.at(materialID).mMaterialInstancedPropertiesArray.resize(size * instancedPropertiesSizeBytes);

    // Reserve index 0 for default material instance
    Slot defaultSlot = mMaterialInstancesDataMap.at(materialID).mSlotsManager.requestSlot();
    mMaterialInstancesDataMap.at(materialID).mMaterialInstancedPropertiesArray.setAt(material->getMaterialData().mSharedMaterialInstancedPropertiesBuffer.getByteBuffer(), defaultSlot.getSlot() * instancedPropertiesSizeBytes);

    const GPUSharedBufferData& instancedPropertiesSharedBufferData = material->getInstancedPropertiesSharedBufferData();
    u32 bindingPointMaterialInstancedProperties = requestSharedBufferBindingPoint(instancedPropertiesSharedBufferData.mType);
    mMaterialInstancesDataMap.at(materialID).mGPUSharedBuffersContainer.createSharedBuffer(bindingPointMaterialInstancedProperties, instancedPropertiesSharedBufferData, false);
    mMaterialInstancesDataMap.at(materialID).mGPUSharedBuffersContainer.getSharedBuffer(instancedPropertiesSharedBufferData).resizeBytes(instancedPropertiesSizeBytes * size);
}

void RenderSharedContext::setMaterialInstanceProperties(const Slot& slot, const MaterialInstance& materialInstance)
{
    PoolHandler<Material> material = materialInstance.mMaterial;
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(mMaterialInstancesDataMap.contains(materialID), "Invalid material!");

    if(material->getMaterialData().mAllowInstances)
    {
        u32 instancedPropertiesSizeBytes = material->getMaterialData().mSharedMaterialInstancedPropertiesBuffer.getByteBuffer().size();
        mMaterialInstancesDataMap.at(materialID).mMaterialInstancedPropertiesArray.setAt(materialInstance.mMaterialInstancedPropertiesBuffer.getByteBuffer(), slot.getSlot() * instancedPropertiesSizeBytes);
    }
}

const GPUSharedBuffer& RenderSharedContext::getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    const GPUSharedBufferData& instancedPropertiesSharedBufferData = material->getInstancedPropertiesSharedBufferData();
    return mMaterialInstancesDataMap.at(material->getID()).mGPUSharedBuffersContainer.getSharedBuffer(instancedPropertiesSharedBufferData);
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