#include "Graphics/Model/Animation/Animation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/RenderEngine.hpp"

void Animation::init(u32 animationIndex, Ptr<const Model> model)
{
    mModel = model;
    mAnimationIndex = animationIndex;

    // const aiScene* m_pScene = mModel.get().mImporter.GetScene();
    // aiAnimation* aiAnimation = m_pScene->mAnimations[mAnimationIndex];

    // mTicksPerSecond = (f32)(aiAnimation->mTicksPerSecond != 0 ? aiAnimation->mTicksPerSecond : 25.0f);
    // mDurationInTicks = aiAnimation->mDuration;
    // mDurationInSeconds = mDurationInTicks / mTicksPerSecond;
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
    mAnimationTime = mAnimation.get().calculateCurrentAnimationTime(mAccumulatedTime);

    f32 dt = GET_SYSTEM(Time).getDeltaTimeSeconds();
    mAccumulatedTime += dt;

    // reset accumulatedTime to avoid overflow
    if(mAccumulatedTime > mAnimation.get().mDurationInSeconds)
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
        mCurrentAnimation.get().update();

        getBoneTransforms(mCurrentBoneTransforms);
    }
}

void SkeletonState::createAnimationState(Ptr<const Animation> animation)
{
    ObjectId animationId = animation.get().getObjectId();

	if(!MAP_CONTAINS(mAnimationStates, animationId))
	{
		OwnerPtr<AnimationState> animationState = OwnerPtr<AnimationState>::newObject();
		animationState.get().init(animation);

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

    f32 animationTime = mCurrentAnimation.get().getAnimationTime();

    Transforms = mCurrentAnimation.get().getAnimation().get().mFrames[animationTime].mTransforms;

    FOR_RANGE(i, 0, mCurrentAnimation.get().getAnimation().get().mModel.get().mBonesIndexCount)
    {
        GET_SYSTEM(RenderEngine).drawCube(Cube(Transforms[i].mulVector(Vector4(0,0,0,1)),Vector3(0.01f,0.01f,0.01f)));
        i32 parentID = mCurrentAnimation.get().getAnimation().get().mModel.get().mBones[i].mParentId;
        if(parentID > 0)
        {
            GET_SYSTEM(RenderEngine).drawLine(Line(Transforms[i].mulVector(Vector4(0,0,0,1)), Transforms[parentID].mulVector(Vector4(0,0,0,1))));
        }
    }
}