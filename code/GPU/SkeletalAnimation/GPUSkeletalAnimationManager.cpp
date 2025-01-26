#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"

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
        it->second.mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mBonesMatrices).setDataArray(transforms);
	}
}

WeakPtr<GPUSkeletonState> GPUSkeletalAnimationManager::createSkeletonState(const GPUSkeletonStateData& gpuSkeletonStateData)
{
	WeakPtr<GPUSkeletonState> skeletonState = *mSkeletonStates.emplace(OwnerPtr<GPUSkeletonState>::newObject()).first;
    skeletonState->init(gpuSkeletonStateData);
    initSkeletonRenderState(skeletonState);
    return skeletonState;
}

void GPUSkeletalAnimationManager::terminate()
{
    FOR_MAP(it, mSkeletonRenderStates)
	{
        it->second.mGPUUniformBuffersContainer.terminate();
	}
	mSkeletonStates.clear();
}

void GPUSkeletalAnimationManager::initSkeletonRenderState(WeakPtr<const GPUSkeletonState> skeletonState)
{
    CHECK_MSG(skeletonState.isValid(), "Invalid skeleton state!");

    SkeletonRenderState skeletonRenderState;
    skeletonRenderState.mGPUUniformBuffersContainer.addUniformBuffer(GPUShaderDefinitions::UniformBuffers::mBonesMatrices, sizeof(Matrix4)*GPUConstants::MAX_BONES, false);

    mSkeletonRenderStates.insert_or_assign(skeletonState, skeletonRenderState);
}

const GPUUniformBuffer& GPUSkeletalAnimationManager::getSkeletonRenderStateGPUUniformBuffer(WeakPtr<const GPUSkeletonState> skeletonState) const
{
    CHECK_MSG(mSkeletonRenderStates.contains(skeletonState), "skeleton state not found!");
    return mSkeletonRenderStates.at(skeletonState).mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mBonesMatrices);
}
