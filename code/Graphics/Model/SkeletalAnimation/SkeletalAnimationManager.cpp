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
		it->second->update();
	}

    FOR_MAP(it, mSkeletonRenderStates)
	{
        const std::vector<Matrix4>& transforms = it->first->getCurrentBoneTransforms();
        it->second.mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).setDataArray(transforms);
	}
}

void SkeletalAnimationManager::createSkeletalAnimationState(Ptr<const SkeletalAnimation> animation)
{
	Ptr<const Model> model = animation->mModel;

	if(!mSkeletonStates.contains(model))
	{
		mSkeletonStates.insert_or_assign(model, OwnerPtr<SkeletonState>::newObject());
		mSkeletonStates.at(model)->init(animation->mModel);
        
        initSkeletonRenderState(mSkeletonStates[model]);
	}

	mSkeletonStates[model]->createSkeletalAnimationState(animation);

}

void SkeletalAnimationManager::terminate()
{
	mSkeletonStates.clear();
}

const std::vector<Matrix4>& SkeletalAnimationManager::getBoneTransforms(Ptr<const Model> model) const
{
	return mSkeletonStates.at(model)->getCurrentBoneTransforms();
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
