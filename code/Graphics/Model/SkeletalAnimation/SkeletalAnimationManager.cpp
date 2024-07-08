#include "Graphics/Model/SkeletalAnimation/SkeletalAnimationManager.hpp"
#include "Graphics/Model/SkeletalAnimation/SkeletalAnimation.hpp"
#include "Graphics/Model/Model.hpp"

void SkeletalAnimationManager::init()
{
}

void SkeletalAnimationManager::update()
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

Ptr<SkeletonState> SkeletalAnimationManager::createSkeletonState()
{
	Ptr<SkeletonState> skeletonState = *mSkeletonStates.emplace(OwnerPtr<SkeletonState>::newObject()).first;
    skeletonState->init();
    initSkeletonRenderState(skeletonState);
    return skeletonState;
}

void SkeletalAnimationManager::createSkeletalAnimationState(Ptr<SkeletonState> skeletonState, Ptr<const SkeletalAnimation> animation)
{
	skeletonState->createSkeletalAnimationState(animation);
}

void SkeletalAnimationManager::terminate()
{
	mSkeletonStates.clear();
}

void SkeletalAnimationManager::initSkeletonRenderState(Ptr<const SkeletonState> skeletonState)
{
    CHECK_MSG(skeletonState.isValid(), "Invalid skeleton state!");

    SkeletonRenderState skeletonRenderState;
    skeletonRenderState.mGPUSharedBuffersContainer.addSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices, false);
    skeletonRenderState.mGPUSharedBuffersContainer.create();
    skeletonRenderState.mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).resize<Matrix4>(GPUBuiltIn::MAX_BONES);

    mSkeletonRenderStates.insert_or_assign(skeletonState, skeletonRenderState);
}

const GPUSharedBuffer& SkeletalAnimationManager::getSkeletonRenderStateGPUSharedBuffer(Ptr<const SkeletonState> skeletonState) const
{
    CHECK_MSG(mSkeletonRenderStates.contains(skeletonState), "skeleton state not found!");
    return mSkeletonRenderStates.at(skeletonState).mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices);
}
