#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"

class GPUSkeletalAnimationManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    WeakPtr<GPUSkeletonState> createSkeletonState(const GPUSkeletonStateData& gpuSkeletonStateData);

    const GPUUniformBuffer& getSkeletonRenderStateGPUUniformBuffer(WeakPtr<const GPUSkeletonState> skeletonState) const;
    WeakPtr<GPUSkeletonState> getSkeletonStateFromMesh(WeakPtr<const GPUMesh> mesh) const;
private:
    void initSkeletonRenderState(WeakPtr<const GPUSkeletonState> skeletonState);

private:
    std::unordered_set<OwnerPtr<GPUSkeletonState>> mSkeletonStates;

    class SkeletonRenderState
    {
    public:
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    };

	std::unordered_map<WeakPtr<const GPUSkeletonState>, SkeletonRenderState> mSkeletonRenderStates;
    std::unordered_map<WeakPtr<const GPUMesh>, WeakPtr<GPUSkeletonState>> mMeshToSkeletonState;

public:
    CRGET(SkeletonStates)
};
REGISTER_CLASS(GPUSkeletalAnimationManager);
