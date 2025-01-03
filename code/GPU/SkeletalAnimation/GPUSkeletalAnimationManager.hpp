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
    Ptr<GPUSkeletonState> createSkeletonState(const GPUSkeletonStateData& gpuSkeletonStateData);

    const GPUUniformBuffer& getSkeletonRenderStateGPUUniformBuffer(Ptr<const GPUSkeletonState> skeletonState) const;
private:
    void initSkeletonRenderState(Ptr<const GPUSkeletonState> skeletonState);

private:
    std::unordered_set<OwnerPtr<GPUSkeletonState>> mSkeletonStates;

    class SkeletonRenderState
    {
    public:
        GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    };

	std::unordered_map<Ptr<const GPUSkeletonState>, SkeletonRenderState> mSkeletonRenderStates;

public:
    CRGET(SkeletonStates)
};
REGISTER_CLASS(GPUSkeletalAnimationManager);
