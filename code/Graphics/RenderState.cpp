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
    mRenderInstancesSlotsManager.reset();
}