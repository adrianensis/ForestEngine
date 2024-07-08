#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Graphics/Model/SkeletalAnimation/SkeletalAnimation.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"

class Model;

class SkeletalAnimationManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void update() override;
    Ptr<SkeletonState> createSkeletonState();
    void createSkeletalAnimationState(Ptr<SkeletonState> skeletonState, Ptr<const SkeletalAnimation> animation);

    const GPUSharedBuffer& getSkeletonRenderStateGPUSharedBuffer(Ptr<const SkeletonState> skeletonState) const;
private:
    void initSkeletonRenderState(Ptr<const SkeletonState> skeletonState);

private:
    std::unordered_set<OwnerPtr<SkeletonState>> mSkeletonStates;

    class SkeletonRenderState
    {
    public:
        GPUSharedBuffersContainer mGPUSharedBuffersContainer;
    };

	std::unordered_map<Ptr<const SkeletonState>, SkeletonRenderState> mSkeletonRenderStates;

public:
    CRGET(SkeletonStates)
};
REGISTER_CLASS(SkeletalAnimationManager);
