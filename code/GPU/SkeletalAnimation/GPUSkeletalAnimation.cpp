#include "GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"

void GPUSkeletalAnimation::init(GPU::u32 id, GPU::f32 animDurationInSeconds)
{
    mID = id;
    mDurationInSeconds = animDurationInSeconds;
    mTicksPerSecond = smSkeletalAnimationFPS;
    mDurationInTicks = (int)(animDurationInSeconds/smSkeletalAnimationFrameRateSeconds);
    mFrames.resize(mDurationInTicks);
}

GPU::f32 GPUSkeletalAnimation::calculateCurrentSkeletalAnimationTime(GPU::f32 accumulatedTime) const
{
    GPU::f32 timeInTicks = accumulatedTime * mTicksPerSecond;
    GPU::f32 animationTime = fmod(timeInTicks, mDurationInTicks);
    return animationTime;
}

void GPUSkeletalAnimationState::init(const GPUSkeletalAnimation* animation)
{
    mSkeletalAnimation = animation;
}

void GPUSkeletalAnimationState::update(GPU::f32 dt)
{
    mSkeletalAnimationTime = mSkeletalAnimation->calculateCurrentSkeletalAnimationTime(mAccumulatedTime);

    GPU::f32 dtSeconds = dt / 1000.0f;
    mAccumulatedTime += dtSeconds;

    // reset accumulatedTime to avoid overflow
    if(mAccumulatedTime > mSkeletalAnimation->mDurationInSeconds)
    {
        mAccumulatedTime = 0;
    }
}

void GPUSkeletonState::init(const GPUSkeletonStateData& gpuSkeletonStateData)
{
    mGPUSkeletonStateData = gpuSkeletonStateData;
}

void GPUSkeletonState::update(GPU::f32 dt)
{
	PROFILER_CPU()

    if(mCurrentSkeletalAnimation)
    {
        mCurrentSkeletalAnimation->update(dt);

        getBoneTransforms(mCurrentBoneTransforms);
    }
}

void GPUSkeletonState::createSkeletalAnimationState(const GPUSkeletalAnimation* animation)
{
    GPU::u32 animationId = animation->mID;

	if(!mSkeletalAnimationStates.contains(animationId))
	{
		mSkeletalAnimationStates.insert_or_assign(animationId, new  GPUSkeletalAnimationState());
		mSkeletalAnimationStates.at(animationId)->init(animation);
	}

    if(! mCurrentSkeletalAnimation)
    {
        mCurrentSkeletalAnimation = mSkeletalAnimationStates.at(animationId);
        
        // generate first bone transforms
        update(0);
    }
}

void GPUSkeletonState::getBoneTransforms(std::vector<Maths::Matrix4>& Transforms) const
{
    getBoneTransformsFromCurrentSkeletalAnimation(Transforms);
}

void GPUSkeletonState::getBoneTransformsFromCurrentSkeletalAnimation(std::vector<Maths::Matrix4>& Transforms) const
{
	PROFILER_CPU()

    Maths::Matrix4 Identity;
	Identity.identity();

    GPU::f32 animationTime = mCurrentSkeletalAnimation->getSkeletalAnimationTime();

    Transforms = mCurrentSkeletalAnimation->getSkeletalAnimation()->mFrames[animationTime].mTransforms;
}