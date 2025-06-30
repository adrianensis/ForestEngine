#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"

class GPUSkeletalAnimationManager: public Core::System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    Core::WeakPtr<GPUSkeletonState> createSkeletonState(const GPUSkeletonStateData& gpuSkeletonStateData);

    const GPUUniformBuffer& getSkeletonRenderStateGPUUniformBuffer(Core::WeakPtr<const GPUSkeletonState> skeletonState) const;
    Core::WeakPtr<GPUSkeletonState> getSkeletonStateFromMesh(Core::WeakPtr<const GPUMesh> mesh) const;
private:
    void initSkeletonRenderState(Core::WeakPtr<const GPUSkeletonState> skeletonState);

private:
    std::unordered_set<Core::OwnerPtr<GPUSkeletonState>> mSkeletonStates;

    class SkeletonRenderState
    {
    public:
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    };

	std::unordered_map<Core::WeakPtr<const GPUSkeletonState>, SkeletonRenderState> mSkeletonRenderStates;
    std::unordered_map<Core::WeakPtr<const GPUMesh>, Core::WeakPtr<GPUSkeletonState>> mMeshToSkeletonState;

public:
    CRGET(SkeletonStates)
};
REGISTER_CLASS(GPUSkeletalAnimationManager);
