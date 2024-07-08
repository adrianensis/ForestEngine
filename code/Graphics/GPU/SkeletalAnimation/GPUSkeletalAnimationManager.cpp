#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

void GPUSkeletalAnimationManager::init()
{
}

void GPUSkeletalAnimationManager::update()
{
	PROFILER_CPU()

	FOR_MAP(it, mSkeletonStates)
	{
		(*it)->update();
	}

    FOR_MAP(it, mSkeletonRenderStates)
	{
        const std::vector<Matrix4>& transforms = it->first->getCurrentBoneTransforms();
        it->second.mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).setDataArray(transforms);
	}
}

Ptr<GPUSkeletonState> GPUSkeletalAnimationManager::createSkeletonState()
{
	Ptr<GPUSkeletonState> skeletonState = *mSkeletonStates.emplace(OwnerPtr<GPUSkeletonState>::newObject()).first;
    skeletonState->init();
    initSkeletonRenderState(skeletonState);
    return skeletonState;
}

void GPUSkeletalAnimationManager::createSkeletalAnimationState(Ptr<GPUSkeletonState> skeletonState, Ptr<const GPUSkeletalAnimation> animation)
{
	skeletonState->createSkeletalAnimationState(animation);
}

void GPUSkeletalAnimationManager::terminate()
{
	mSkeletonStates.clear();
}

void GPUSkeletalAnimationManager::initSkeletonRenderState(Ptr<const GPUSkeletonState> skeletonState)
{
    CHECK_MSG(skeletonState.isValid(), "Invalid skeleton state!");

    SkeletonRenderState skeletonRenderState;
    skeletonRenderState.mGPUSharedBuffersContainer.addSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices, false);
    skeletonRenderState.mGPUSharedBuffersContainer.create();
    skeletonRenderState.mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).resize<Matrix4>(GPUBuiltIn::MAX_BONES);

    mSkeletonRenderStates.insert_or_assign(skeletonState, skeletonRenderState);
}

const GPUSharedBuffer& GPUSkeletalAnimationManager::getSkeletonRenderStateGPUSharedBuffer(Ptr<const GPUSkeletonState> skeletonState) const
{
    CHECK_MSG(mSkeletonRenderStates.contains(skeletonState), "skeleton state not found!");
    return mSkeletonRenderStates.at(skeletonState).mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices);
}
