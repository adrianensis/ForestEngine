#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimation.hpp"

void GPUSkeletalAnimation::init(f32 animDurationInSeconds)
{
    mDurationInSeconds = animDurationInSeconds;
    mTicksPerSecond = smSkeletalAnimationFPS;
    mDurationInTicks = (int)(animDurationInSeconds/smSkeletalAnimationFrameRateSeconds);
    mFrames.resize(mDurationInTicks);
}

f32 GPUSkeletalAnimation::calculateCurrentSkeletalAnimationTime(f32 accumulatedTime) const
{
    f32 timeInTicks = accumulatedTime * mTicksPerSecond;
    f32 animationTime = fmod(timeInTicks, mDurationInTicks);
    return animationTime;
}

void GPUSkeletalAnimationState::init(Ptr<const GPUSkeletalAnimation> animation)
{
    mSkeletalAnimation = animation;
}

void GPUSkeletalAnimationState::update()
{
    mSkeletalAnimationTime = mSkeletalAnimation->calculateCurrentSkeletalAnimationTime(mAccumulatedTime);

    f32 dt = GET_SYSTEM(Time).getDeltaTimeSeconds();
    mAccumulatedTime += dt;

    // reset accumulatedTime to avoid overflow
    if(mAccumulatedTime > mSkeletalAnimation->mDurationInSeconds)
    {
        mAccumulatedTime = 0;
    }
}

void GPUSkeletonState::init()
{
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

void GPUSkeletonState::createSkeletalAnimationState(Ptr<const GPUSkeletalAnimation> animation)
{
    ObjectId animationId = animation->getObjectId();

	if(!mSkeletalAnimationStates.contains(animationId))
	{
		mSkeletalAnimationStates.insert_or_assign(animationId, OwnerPtr<GPUSkeletalAnimationState>::newObject());
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

    f32 animationTime = mCurrentSkeletalAnimation->getSkeletalAnimationTime();

    Transforms = mCurrentSkeletalAnimation->getSkeletalAnimation()->mFrames[animationTime].mTransforms;
}