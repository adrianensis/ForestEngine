#pragma once

#include "Core/Core.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"

class GPUSkeletalAnimationManager
{
public:
    void init();
    void terminate();
    void update(GPU::f32 dt);
    GPUSkeletonState* createSkeletonState(GPUContext* gpuContext, const GPUSkeletonStateData& gpuSkeletonStateData);

    const GPUUniformBuffer& getSkeletonRenderStateGPUUniformBuffer(const GPUSkeletonState* skeletonState) const;
    GPUSkeletonState* getSkeletonStateFromMesh(const GPUMesh* mesh) const;
private:
    void initSkeletonRenderState(GPUContext* gpuContext, const GPUSkeletonState* skeletonState);

private:
    std::unordered_set<GPUSkeletonState*> mSkeletonStates;

    class SkeletonRenderState
    {
    public:
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    };

	std::unordered_map<const GPUSkeletonState*, SkeletonRenderState> mSkeletonRenderStates;
    std::unordered_map<const GPUMesh*, GPUSkeletonState*> mMeshToSkeletonState;

public:
    const auto& getSkeletonStates() const { return mSkeletonStates; }
};

