#pragma once

#include "Core/Core.hpp"
#include "GPU/Core/GPUDefinitions.h"

class GPUMesh;
class Frame
{
public:
    std::vector<Maths::Matrix4> mTransforms;
};

class GPUSkeletalAnimation
{
public:
    void init(GPU::u32 id, GPU::f32 animDurationInSeconds);

    GPU::f32 calculateCurrentSkeletalAnimationTime(GPU::f32 accumulatedTime) const;

public:
    inline static const float smSkeletalAnimationFPS = 60.0f;
    inline static const float smSkeletalAnimationFrameRateSeconds = 1.0f/smSkeletalAnimationFPS;
    
public:
    std::vector<Frame> mFrames;
    GPU::f32 mTicksPerSecond = 0;
    GPU::u32 mDurationInTicks = 0;
    GPU::f32 mDurationInSeconds = 0;
    GPU::u32 mID = 0;
};


class GPUSkeletalAnimationState
{
public:
    void init(Core::WeakPtr<const GPUSkeletalAnimation> animation);
    void update(GPU::f32 dt);

private:
    Core::WeakPtr<const GPUSkeletalAnimation> mSkeletalAnimation;
    GPU::f32 mAccumulatedTime = 0;
    GPU::f32 mSkeletalAnimationTime = 0;

public:
    CGET(SkeletalAnimation)
    GET(SkeletalAnimationTime)
};


class GPUBoneData
{
public:

    GPU::i32 mId = INVALID_INDEX;
    GPU::i32 mParentId = INVALID_INDEX;
    std::string mName;
    // offset matrix transforms vertex from model space to bone space
	Maths::Matrix4 mBindMatrix;
};

class GPUSkeletonStateData
{
public:
    std::vector<GPUBoneData> mBones;
    std::vector<Maths::Matrix4> mInverseBindMatrices;
    std::vector<Core::WeakPtr<const GPUMesh>> mMeshes;
};

class GPUSkeletonState
{
public:
    void init(const GPUSkeletonStateData& gpuSkeletonStateData);
    void createSkeletalAnimationState(Core::WeakPtr<const GPUSkeletalAnimation> animation);
    void update(GPU::f32 dt);

private:
    void getBoneTransforms(std::vector<Maths::Matrix4>& Transforms) const;
    void getBoneTransformsFromCurrentSkeletalAnimation(std::vector<Maths::Matrix4>& Transforms) const;
private:
    GPUSkeletonStateData mGPUSkeletonStateData;
    std::unordered_map<GPU::u32, Core::OwnerPtr<GPUSkeletalAnimationState>> mSkeletalAnimationStates;
    Core::WeakPtr<GPUSkeletalAnimationState> mCurrentSkeletalAnimation;
    std::vector<Maths::Matrix4> mCurrentBoneTransforms;

public:
    CRGET(CurrentBoneTransforms)
    CRGET(GPUSkeletonStateData)
};

