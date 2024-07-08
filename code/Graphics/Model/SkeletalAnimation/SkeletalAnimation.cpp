#include "Graphics/Model/SkeletalAnimation/SkeletalAnimation.hpp"
#include "Graphics/Model/Model.hpp"

void SkeletalAnimation::init(f32 animDurationInSeconds)
{
    mDurationInSeconds = animDurationInSeconds;
    mTicksPerSecond = Model::smSkeletalAnimationFPS;
    mDurationInTicks = (int)(animDurationInSeconds/Model::smSkeletalAnimationFrameRateSeconds);
    mFrames.resize(mDurationInTicks);
}

f32 SkeletalAnimation::calculateCurrentSkeletalAnimationTime(f32 accumulatedTime) const
{
    f32 timeInTicks = accumulatedTime * mTicksPerSecond;
    f32 animationTime = fmod(timeInTicks, mDurationInTicks);
    return animationTime;
}

void SkeletalAnimationState::init(Ptr<const SkeletalAnimation> animation)
{
    mSkeletalAnimation = animation;
}

void SkeletalAnimationState::update()
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

void SkeletonState::init()
{
}

void SkeletonState::update()
{
	PROFILER_CPU()

    if(mCurrentSkeletalAnimation)
    {
        mCurrentSkeletalAnimation->update();

        getBoneTransforms(mCurrentBoneTransforms);
    }
}

void SkeletonState::createSkeletalAnimationState(Ptr<const SkeletalAnimation> animation)
{
    ObjectId animationId = animation->getObjectId();

	if(!mSkeletalAnimationStates.contains(animationId))
	{
		mSkeletalAnimationStates.insert_or_assign(animationId, OwnerPtr<SkeletalAnimationState>::newObject());
		mSkeletalAnimationStates.at(animationId)->init(animation);
	}

    if(! mCurrentSkeletalAnimation)
    {
        mCurrentSkeletalAnimation = mSkeletalAnimationStates.at(animationId);
        
        // generate first bone transforms
        update();
    }
}

void SkeletonState::getBoneTransforms(std::vector<Matrix4>& Transforms) const
{
    getBoneTransformsFromCurrentSkeletalAnimation(Transforms);
}

void SkeletonState::getBoneTransformsFromCurrentSkeletalAnimation(std::vector<Matrix4>& Transforms) const
{
	PROFILER_CPU()

    Matrix4 Identity;
	Identity.identity();

    f32 animationTime = mCurrentSkeletalAnimation->getSkeletalAnimationTime();

    Transforms = mCurrentSkeletalAnimation->getSkeletalAnimation()->mFrames[animationTime].mTransforms;
}