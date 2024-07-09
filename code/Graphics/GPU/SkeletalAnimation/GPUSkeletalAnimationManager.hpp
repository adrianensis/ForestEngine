#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"

class GPUSkeletalAnimationManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void update() override;
    Ptr<GPUSkeletonState> createSkeletonState(const GPUSkeletonStateData& gpuSkeletonStateData);

    const GPUSharedBuffer& getSkeletonRenderStateGPUSharedBuffer(Ptr<const GPUSkeletonState> skeletonState) const;
private:
    void initSkeletonRenderState(Ptr<const GPUSkeletonState> skeletonState);

private:
    std::unordered_set<OwnerPtr<GPUSkeletonState>> mSkeletonStates;

    class SkeletonRenderState
    {
    public:
        GPUSharedBuffersContainer mGPUSharedBuffersContainer;
    };

	std::unordered_map<Ptr<const GPUSkeletonState>, SkeletonRenderState> mSkeletonRenderStates;

public:
    CRGET(SkeletonStates)
};
REGISTER_CLASS(GPUSkeletalAnimationManager);
