#pragma once

#include "Engine/Minimal.hpp"

class Model;

class Frame
{
public:
    std::vector<Matrix4> mTransforms;
};

class SkeletalAnimation: public ObjectBase
{
public:
    void init(f32 animDurationInSeconds, Ptr<const Model> model);

    f32 calculateCurrentSkeletalAnimationTime(f32 accumulatedTime) const;

public:
    Ptr<const Model> mModel;
    std::vector<Frame> mFrames;
    f32 mTicksPerSecond = 0;
    u32 mDurationInTicks = 0;
    f32 mDurationInSeconds = 0;
};
REGISTER_CLASS(SkeletalAnimation);

class SkeletalAnimationState: public ObjectBase
{
public:
    void init(Ptr<const SkeletalAnimation> animation);
    void update();

private:
    Ptr<const SkeletalAnimation> mSkeletalAnimation;
    f32 mAccumulatedTime = 0;
    f32 mSkeletalAnimationTime = 0;

public:
    CGET(SkeletalAnimation)
    GET(SkeletalAnimationTime)
};
REGISTER_CLASS(SkeletalAnimationState);

class SkeletonState: public ObjectBase
{
public:
    void init(Ptr<const Model> model);
    void createSkeletalAnimationState(Ptr<const SkeletalAnimation> animation);
    void update();

private:
    void getBoneTransforms(std::vector<Matrix4>& Transforms) const;
    void getBoneTransformsFromCurrentSkeletalAnimation(std::vector<Matrix4>& Transforms) const;
private:
    Ptr<const Model> mModel;
    std::unordered_map<ObjectId, OwnerPtr<SkeletalAnimationState>> mSkeletalAnimationStates;
    Ptr<SkeletalAnimationState> mCurrentSkeletalAnimation;
    std::vector<Matrix4> mCurrentBoneTransforms;

public:
    CRGET(CurrentBoneTransforms)
};
REGISTER_CLASS(SkeletonState);
