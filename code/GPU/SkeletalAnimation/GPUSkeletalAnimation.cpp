#include "GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"
#include "Engine/Time/TimeUtils.hpp"

void GPUSkeletalAnimation::init(Core::u32 id, Core::f32 animDurationInSeconds)
{
    mID = id;
    mDurationInSeconds = animDurationInSeconds;
    mTicksPerSecond = smSkeletalAnimationFPS;
    mDurationInTicks = (int)(animDurationInSeconds/smSkeletalAnimationFrameRateSeconds);
    mFrames.resize(mDurationInTicks);
}

Core::f32 GPUSkeletalAnimation::calculateCurrentSkeletalAnimationTime(Core::f32 accumulatedTime) const
{
    Core::f32 timeInTicks = accumulatedTime * mTicksPerSecond;
    Core::f32 animationTime = fmod(timeInTicks, mDurationInTicks);
    return animationTime;
}

void GPUSkeletalAnimationState::init(Core::WeakPtr<const GPUSkeletalAnimation> animation)
{
    mSkeletalAnimation = animation;
}

void GPUSkeletalAnimationState::update()
{
    mSkeletalAnimationTime = mSkeletalAnimation->calculateCurrentSkeletalAnimationTime(mAccumulatedTime);

    Core::f32 dt = GET_SYSTEM(Time::Time).getDeltaTimeSeconds();
    mAccumulatedTime += dt;

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

void GPUSkeletonState::update()
{
	PROFILER_CPU()

    if(mCurrentSkeletalAnimation)
    {
        mCurrentSkeletalAnimation->update();

        getBoneTransforms(mCurrentBoneTransforms);
    }
}

void GPUSkeletonState::createSkeletalAnimationState(Core::WeakPtr<const GPUSkeletalAnimation> animation)
{
    Core::u32 animationId = animation->mID;

	if(!mSkeletalAnimationStates.contains(animationId))
	{
		mSkeletalAnimationStates.insert_or_assign(animationId, Core::OwnerPtr<GPUSkeletalAnimationState>::newObject());
		mSkeletalAnimationStates.at(animationId)->init(animation);
	}

    if(! mCurrentSkeletalAnimation)
    {
        mCurrentSkeletalAnimation = mSkeletalAnimationStates.at(animationId);
        
        // generate first bone transforms
        update();
    }
}

void GPUSkeletonState::getBoneTransforms(std::vector<Matrix4>& Transforms) const
{
    getBoneTransformsFromCurrentSkeletalAnimation(Transforms);
}

void GPUSkeletonState::getBoneTransformsFromCurrentSkeletalAnimation(std::vector<Matrix4>& Transforms) const
{
	PROFILER_CPU()

    Matrix4 Identity;
	Identity.identity();

    Core::f32 animationTime = mCurrentSkeletalAnimation->getSkeletalAnimationTime();

    Transforms = mCurrentSkeletalAnimation->getSkeletalAnimation()->mFrames[animationTime].mTransforms;
}