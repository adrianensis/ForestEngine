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

    FOR_MAP(it, mMaterialRenderStates)
    {
        PoolHandler<Material> material = it->second.mMaterial;
        const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;

        ByteBuffer& materialPropertiesBlockArray = it->second.mMaterialPropertiesBlockArray;
	    it->second.mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData).setDataArray(materialPropertiesBlockArray);
    }

    FOR_MAP(it, mSkeletonRenderStates)
	{
        const std::vector<Matrix4>& transforms = it->first->getCurrentBoneTransforms();
        it->second.mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).setDataArray(transforms);
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

void RenderState::setMaterialInstanceProperties(const Slot& slot, const MaterialInstance& materialInstance)
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

const GPUSharedBuffer& RenderState::getMaterialPropertiesGPUSharedBuffer(const PoolHandler<Material>& material) const
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    const GPUSharedBufferData& propertiesBlockSharedBufferData = material->getShader()->getShaderData().mPropertiesBlockSharedBufferData;
    return mMaterialRenderStates.at(material->getID()).mGPUSharedBuffersContainer.getSharedBuffer(propertiesBlockSharedBufferData);
}

Slot RenderState::requestMaterialInstanceSlot(const PoolHandler<Material>& material)
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

void RenderState::freeMaterialInstanceSlot(const PoolHandler<Material>& material, Slot& slot)
{
    CHECK_MSG(material.isValid(), "Invalid material!");
    if(material->getMaterialData().mAllowInstances)
    {
        mMaterialRenderStates.at(material->getID()).mSlotsManager.freeSlot(slot);
    }
}

void RenderState::initSkeletonRenderState(Ptr<const SkeletonState> skeletonState)
{
    CHECK_MSG(skeletonState.isValid(), "Invalid skeleton state!");

    SkeletonRenderState skeletonRenderState;
    skeletonRenderState.mGPUSharedBuffersContainer.addSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices, false);
    skeletonRenderState.mGPUSharedBuffersContainer.create();
    skeletonRenderState.mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).resize<Matrix4>(GPUBuiltIn::MAX_BONES);

    mSkeletonRenderStates.insert_or_assign(skeletonState, skeletonRenderState);
}

const GPUSharedBuffer& RenderState::getSkeletonRenderStateGPUSharedBuffer(Ptr<const SkeletonState> skeletonState) const
{
    CHECK_MSG(mSkeletonRenderStates.contains(skeletonState), "skeleton state not found!");
    return mSkeletonRenderStates.at(skeletonState).mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices);
}

void RenderState::terminate()
{
    FOR_MAP(it, mMaterialRenderStates)
    {
        it->second.mGPUSharedBuffersContainer.terminate();
    }

    mMaterialRenderStates.clear();
    mRenderInstancesSlotsManager.reset();
}