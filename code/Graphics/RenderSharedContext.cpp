#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Light/Light.hpp"

void RenderSharedContext::init()
{
    mMaxSharedBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForSharedBuffer(GPUBufferType::UNIFORM);
    mMaxSharedBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForSharedBuffer(GPUBufferType::STORAGE);

    u32 bindingPointGlobalData = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mGlobalData.mType);
    mGPUSharedBuffersContainer.addSharedBuffer(bindingPointGlobalData, GPUBuiltIn::SharedBuffers::mGlobalData, false);

    u32 bindingPointLightsData = requestSharedBufferBindingPoint(LightBuiltIn::mLightsBufferData.mType);
    mGPUSharedBuffersContainer.addSharedBuffer(bindingPointLightsData, LightBuiltIn::mLightsBufferData, false);

    u32 bindingPointShadowMappingData = requestSharedBufferBindingPoint(LightBuiltIn::mShadowMappingBufferData.mType);
    mGPUSharedBuffersContainer.addSharedBuffer(bindingPointShadowMappingData, LightBuiltIn::mShadowMappingBufferData, false);

    mRenderInstancesSlotsManager.init(mMaxInstances);

    mMatrices.resize(mRenderInstancesSlotsManager.getSize());
    u32 bindingPointModelMatrices = requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mModelMatrices.mType);
    mGPUSharedBuffersContainer.addSharedBuffer(bindingPointModelMatrices, GPUBuiltIn::SharedBuffers::mModelMatrices, false);

    mGPUSharedBuffersContainer.create();
    mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).resize<GPUBuiltIn::SharedBuffers::GPUGlobalData>(1);
    mGPUSharedBuffersContainer.getSharedBuffer(LightBuiltIn::mLightsBufferData).resize<LightBuiltIn::LightsData>(1);
    mGPUSharedBuffersContainer.getSharedBuffer(LightBuiltIn::mShadowMappingBufferData).resize<LightBuiltIn::ShadowMappingData>(1);
    mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
}

void RenderSharedContext::update()
{
	PROFILER_CPU()
	mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).setDataArray(mMatrices);

    FOR_MAP(it, mMaterialInstancesDataMap)
    {
        PoolHandler<Material> material = it->second.mMaterial;
        const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getMaterialRuntime()->getMaterialRuntimeData().mPropertiesBlockSharedBufferData;

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
    CHECK_MSG(slot.isValid(), "Invalid slot!");
    mMatrices.at(slot.getSlot()) = matrix;
}

void RenderSharedContext::initMaterialInstancePropertiesSharedBuffer(const PoolHandler<Material>& material)
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

    const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getMaterialRuntime()->getMaterialRuntimeData().mPropertiesBlockSharedBufferData;
    u32 bindingPointMaterialPropertiesBlock = requestSharedBufferBindingPoint(propertiesBlockSharedBufferData.mType);
    mMaterialInstancesDataMap.at(materialID).mGPUSharedBuffersContainer.addSharedBuffer(bindingPointMaterialPropertiesBlock, propertiesBlockSharedBufferData, false);
    mMaterialInstancesDataMap.at(materialID).mGPUSharedBuffersContainer.create();
    mMaterialInstancesDataMap.at(materialID).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData).resizeBytes(propertiesBlockSizeBytes * size);
}

void RenderSharedContext::setMaterialInstanceProperties(const Slot& slot, const MaterialInstance& materialInstance)
{
    PoolHandler<Material> material = materialInstance.mMaterial;
    CHECK_MSG(material.isValid(), "Invalid material!");
    u32 materialID = material->getID();
    CHECK_MSG(mMaterialInstancesDataMap.contains(materialID), "Invalid material!");

    if(material->getMaterialData().allowInstances())
    {
        u32 propertiesBlockSizeBytes = material->getMaterialData().getSharedMaterialPropertiesBlockBufferSize();
        mMaterialInstancesDataMap.at(materialID).mMaterialPropertiesBlockArray.setAt(materialInstance.mMaterialPropertiesBlockBuffer.getByteBuffer(), slot.getSlot() * propertiesBlockSizeBytes);
    }
}

const GPUSharedBuffer& RenderSharedContext::getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getMaterialRuntime()->getMaterialRuntimeData().mPropertiesBlockSharedBufferData;
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
        // point to default material instance
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