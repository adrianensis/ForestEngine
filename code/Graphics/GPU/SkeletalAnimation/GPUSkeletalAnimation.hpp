#pragma once

#include "Engine/Minimal.hpp"

class Frame
{
public:
    std::vector<Matrix4> mTransforms;
};

class GPUSkeletalAnimation: public ObjectBase
{
public:
    void init(f32 animDurationInSeconds);

    f32 calculateCurrentSkeletalAnimationTime(f32 accumulatedTime) const;

public:
    inline static const float smSkeletalAnimationFPS = 60.0f;
    inline static const float smSkeletalAnimationFrameRateSeconds = 1.0f/smSkeletalAnimationFPS;
    
public:
    std::vector<Frame> mFrames;
    f32 mTicksPerSecond = 0;
    u32 mDurationInTicks = 0;
    f32 mDurationInSeconds = 0;
};
REGISTER_CLASS(GPUSkeletalAnimation);

class GPUSkeletalAnimationState: public ObjectBase
{
public:
    void init(Ptr<const GPUSkeletalAnimation> animation);
    void update();

private:
    Ptr<const GPUSkeletalAnimation> mSkeletalAnimation;
    f32 mAccumulatedTime = 0;
    f32 mSkeletalAnimationTime = 0;

public:
    CGET(SkeletalAnimation)
    GET(SkeletalAnimationTime)
};
REGISTER_CLASS(GPUSkeletalAnimationState);

class GPUSkeletonState: public ObjectBase
{
public:
    void init();
    void createSkeletalAnimationState(Ptr<const GPUSkeletalAnimation> animation);
    void update();

private:
    void getBoneTransforms(std::vector<Matrix4>& Transforms) const;
    void getBoneTransformsFromCurrentSkeletalAnimation(std::vector<Matrix4>& Transforms) const;
private:
    std::unordered_map<ObjectId, OwnerPtr<GPUSkeletalAnimationState>> mSkeletalAnimationStates;
    Ptr<GPUSkeletalAnimationState> mCurrentSkeletalAnimation;
    std::vector<Matrix4> mCurrentBoneTransforms;

public:
    CRGET(CurrentBoneTransforms)
};
REGISTER_CLASS(GPUSkeletonState);
