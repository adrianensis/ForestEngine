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
        const std::vector<Maths::Matrix4>& transforms = it->first->getCurrentBoneTransforms();
        it->second.mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mBonesMatrices).setDataArray(transforms);
	}
}

Core::WeakPtr<GPUSkeletonState> GPUSkeletalAnimationManager::createSkeletonState(GPUContext* gpuContext, const GPUSkeletonStateData& gpuSkeletonStateData)
{
	Core::WeakPtr<GPUSkeletonState> skeletonState = *mSkeletonStates.emplace(Core::OwnerPtr<GPUSkeletonState>::newObject()).first;
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

void GPUSkeletalAnimationManager::initSkeletonRenderState(GPUContext* gpuContext, Core::WeakPtr<const GPUSkeletonState> skeletonState)
{
    CHECK_MSG(skeletonState.isValid(), "Invalid skeleton state!");

    SkeletonRenderState skeletonRenderState;
    skeletonRenderState.mGPUUniformBuffersContainer.addUniformBuffer(gpuContext, GPUShaderDefinitions::UniformBuffers::mBonesMatrices, sizeof(Maths::Matrix4)*GPUConstants::MAX_BONES, false);

    mSkeletonRenderStates.insert_or_assign(skeletonState, skeletonRenderState);
}

const GPUUniformBuffer& GPUSkeletalAnimationManager::getSkeletonRenderStateGPUUniformBuffer(Core::WeakPtr<const GPUSkeletonState> skeletonState) const
{
    CHECK_MSG(mSkeletonRenderStates.contains(skeletonState), "skeleton state not found!");
    return mSkeletonRenderStates.at(skeletonState).mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mBonesMatrices);
}

Core::WeakPtr<GPUSkeletonState> GPUSkeletalAnimationManager::getSkeletonStateFromMesh(Core::WeakPtr<const GPUMesh> mesh) const
{
    Core::WeakPtr<GPUSkeletonState> result;
    if(mMeshToSkeletonState.contains(mesh))
    {
        result = mMeshToSkeletonState.at(mesh);
    }
    return result;
}
