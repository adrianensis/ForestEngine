#pragma once

#include "Core/Module.hpp"

class Model;

class Frame
{
public:
    std::vector<Matrix4> mTransforms;
};

class Animation: public ObjectBase
{
    GENERATE_METADATA(Animation)

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

class AnimationState: public ObjectBase
{
    GENERATE_METADATA(AnimationState)

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

class SkeletonState: public ObjectBase
{
    GENERATE_METADATA(SkeletonState)

public:
    void init(Ptr<const Model> model);
    void createAnimationState(Ptr<const Animation> animation);
    void update();

private:
    void getBoneTransforms(std::vector<Matrix4>& Transforms) const;
    void getBoneTransformsFromCurrentAnimation(std::vector<Matrix4>& Transforms) const;
private:
    Ptr<const Model> mModel;
    std::unordered_map<ObjectId, SharedPtr<AnimationState>> mAnimationStates;
    Ptr<AnimationState> mCurrentAnimation;
    std::vector<Matrix4> mCurrentBoneTransforms;

public:
    CRGET(CurrentBoneTransforms)
};
