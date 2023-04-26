#include "Graphics/Model/Animation/Animation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/RenderEngine.hpp"

void Animation::init(f32 animDurationInSeconds, Ptr<const Model> model)
{
    mModel = model;

    mDurationInSeconds = animDurationInSeconds;
    mTicksPerSecond = Model::smAnimationFPS;
    mDurationInTicks = (int)(animDurationInSeconds/Model::smAnimationFrameRateSeconds);
    mFrames.resize(mDurationInTicks);
}

f32 Animation::calculateCurrentAnimationTime(f32 accumulatedTime) const
{
    f32 timeInTicks = accumulatedTime * mTicksPerSecond;
    f32 animationTime = fmod(timeInTicks, mDurationInTicks);
    return animationTime;
}

void AnimationState::init(Ptr<const Animation> animation)
{
    mAnimation = animation;
}

void AnimationState::update()
{
    mAnimationTime = mAnimation->calculateCurrentAnimationTime(mAccumulatedTime);

    f32 dt = GET_SYSTEM(Time).getDeltaTimeSeconds();
    mAccumulatedTime += dt;

    // reset accumulatedTime to avoid overflow
    if(mAccumulatedTime > mAnimation->mDurationInSeconds)
    {
        mAccumulatedTime = 0;
    }
}

void SkeletonState::init(Ptr<const Model> model)
{
    mModel = model;
}

void SkeletonState::update()
{
	PROFILER_CPU()

    if(mCurrentAnimation)
    {
        mCurrentAnimation->update();

        getBoneTransforms(mCurrentBoneTransforms);
    }
}

void SkeletonState::createAnimationState(Ptr<const Animation> animation)
{
    ObjectId animationId = animation->getObjectId();

	if(!MAP_CONTAINS(mAnimationStates, animationId))
	{
		SharedPtr<AnimationState> animationState = SharedPtr<AnimationState>::newObject();
		animationState->init(animation);

		MAP_INSERT(mAnimationStates, animationId, animationState);
	}

    // TMP: grab the first anim state for testing
    if(! mCurrentAnimation)
    {
        mCurrentAnimation = mAnimationStates.at(animationId);
        
        // generate first bone transforms
        update();
    }
}

void SkeletonState::getBoneTransforms(std::vector<Matrix4>& Transforms) const
{
    getBoneTransformsFromCurrentAnimation(Transforms);
}

void SkeletonState::getBoneTransformsFromCurrentAnimation(std::vector<Matrix4>& Transforms) const
{
	PROFILER_CPU()

    Matrix4 Identity;
	Identity.identity();

    f32 animationTime = mCurrentAnimation->getAnimationTime();

    Transforms = mCurrentAnimation->getAnimation()->mFrames[animationTime].mTransforms;
}