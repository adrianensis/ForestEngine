#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "Graphics/GPU/Core/GPUBuiltIn.hpp"

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
        it->second.mGPUUniformBuffersContainer.getUniformBuffer(GPUBuiltIn::UniformBuffers::mBonesMatrices).setDataArray(transforms);
	}
}

Ptr<GPUSkeletonState> GPUSkeletalAnimationManager::createSkeletonState(const GPUSkeletonStateData& gpuSkeletonStateData)
{
	Ptr<GPUSkeletonState> skeletonState = *mSkeletonStates.emplace(OwnerPtr<GPUSkeletonState>::newObject()).first;
    skeletonState->init(gpuSkeletonStateData);
    initSkeletonRenderState(skeletonState);
    return skeletonState;
}

void GPUSkeletalAnimationManager::terminate()
{
	mSkeletonStates.clear();
}

void GPUSkeletalAnimationManager::initSkeletonRenderState(Ptr<const GPUSkeletonState> skeletonState)
{
    CHECK_MSG(skeletonState.isValid(), "Invalid skeleton state!");

    SkeletonRenderState skeletonRenderState;
    skeletonRenderState.mGPUUniformBuffersContainer.addUniformBuffer(GPUBuiltIn::UniformBuffers::mBonesMatrices, sizeof(Matrix4)*GPUBuiltIn::MAX_BONES, false);
    skeletonRenderState.mGPUUniformBuffersContainer.create();

    mSkeletonRenderStates.insert_or_assign(skeletonState, skeletonRenderState);
}

const GPUUniformBuffer& GPUSkeletalAnimationManager::getSkeletonRenderStateGPUUniformBuffer(Ptr<const GPUSkeletonState> skeletonState) const
{
    CHECK_MSG(mSkeletonRenderStates.contains(skeletonState), "skeleton state not found!");
    return mSkeletonRenderStates.at(skeletonState).mGPUUniformBuffersContainer.getUniformBuffer(GPUBuiltIn::UniformBuffers::mBonesMatrices);
}
