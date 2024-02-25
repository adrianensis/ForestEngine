#pragma once

#include "Engine/Minimal.hpp"

class Model;

class Frame
{
public:
    std::vector<Matrix4> mTransforms;
};

class Animation: public ObjectBase
{
    

public:
    void init(f32 animDurationInSeconds, Ptr<const Model> model);

    f32 calculateCurrentAnimationTime(f32 accumulatedTime) const;

public:
    Ptr<const Model> mModel;
    std::vector<Frame> mFrames;
    f32 mTicksPerSecond = 0;
    u32 mDurationInTicks = 0;
    f32 mDurationInSeconds = 0;
};
REGISTER_CLASS(Animation);

class AnimationState: public ObjectBase
{
    

public:
    void init(Ptr<const Animation> animation);
    void update();

private:
    Ptr<const Animation> mAnimation;
    f32 mAccumulatedTime = 0;
    f32 mAnimationTime = 0;

public:
    CGET(Animation)
    GET(AnimationTime)
};
REGISTER_CLASS(AnimationState);

class SkeletonState: public ObjectBase
{
    

public:
    void init(Ptr<const Model> model);
    void createAnimationState(Ptr<const Animation> animation);
    void update();

private:
    void getBoneTransforms(std::vector<Matrix4>& Transforms) const;
    void getBoneTransformsFromCurrentAnimation(std::vector<Matrix4>& Transforms) const;
private:
    Ptr<const Model> mModel;
    std::unordered_map<ObjectId, OwnerPtr<AnimationState>> mAnimationStates;
    Ptr<AnimationState> mCurrentAnimation;
    std::vector<Matrix4> mCurrentBoneTransforms;

public:
    CRGET(CurrentBoneTransforms)
};
REGISTER_CLASS(SkeletonState);
