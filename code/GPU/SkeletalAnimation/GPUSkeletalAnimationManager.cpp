#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"

void GPUSkeletalAnimationManager::init()
{
}

void GPUSkeletalAnimationManager::update(GPU::f32 dt)
{
	PROFILER_CPU()

	FOR_MAP(it, mSkeletonStates)
	{
		(*it)->update(dt);
	}

    FOR_MAP(it, mSkeletonRenderStates)
	{
        const std::vector<Maths::Matrix4>& transforms = it->first->getCurrentBoneTransforms();
        it->second.mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mBonesMatrices).setDataArray(transforms);
	}
}

GPUSkeletonState* GPUSkeletalAnimationManager::createSkeletonState(GPUContext* gpuContext, const GPUSkeletonStateData& gpuSkeletonStateData)
{
	GPUSkeletonState* skeletonState = *mSkeletonStates.emplace( new GPUSkeletonState()).first;
    skeletonState->init(gpuSkeletonStateData);

    FOR_LIST(it, gpuSkeletonStateData.mMeshes)
    {
        mMeshToSkeletonState.insert({*it, skeletonState});
    }

    initSkeletonRenderState(gpuContext, skeletonState);
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

void GPUSkeletalAnimationManager::initSkeletonRenderState(GPUContext* gpuContext, const GPUSkeletonState* skeletonState)
{
    CHECK_MSG(skeletonState, "Invalid skeleton state!");

    SkeletonRenderState skeletonRenderState;
    skeletonRenderState.mGPUUniformBuffersContainer.addUniformBuffer(gpuContext, GPUShaderDefinitions::UniformBuffers::mBonesMatrices, sizeof(Maths::Matrix4)*GPUConstants::MAX_BONES, false);

    mSkeletonRenderStates.insert_or_assign(skeletonState, skeletonRenderState);
}

const GPUUniformBuffer& GPUSkeletalAnimationManager::getSkeletonRenderStateGPUUniformBuffer(const GPUSkeletonState* skeletonState) const
{
    CHECK_MSG(mSkeletonRenderStates.contains(skeletonState), "skeleton state not found!");
    return mSkeletonRenderStates.at(skeletonState).mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mBonesMatrices);
}

GPUSkeletonState* GPUSkeletalAnimationManager::getSkeletonStateFromMesh(const GPUMesh* mesh) const
{
    GPUSkeletonState* result = nullptr;
    if(mMeshToSkeletonState.contains(mesh))
    {
        result = mMeshToSkeletonState.at(mesh);
    }
    return result;
}
