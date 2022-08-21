#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Animation/Animation.hpp"
#include "Graphics/Model/Model.hpp"

void AnimationManager::init()
{
}

void AnimationManager::update()
{
	PROFILER_CPU()

	FOR_MAP(it, mSkeletonStates)
	{
		it->second.get().update();
	}
}

void AnimationManager::createAnimationState(Ptr<const Animation> animation)
{
	ObjectId modelId = animation.get().getModel().get().getObjectId();

	if(!MAP_CONTAINS(mSkeletonStates, modelId))
	{
		OwnerPtr<SkeletonState> skeletonState = OwnerPtr<SkeletonState>(NEW(SkeletonState));
		skeletonState.get().init(animation.get().getModel());

		MAP_INSERT(mSkeletonStates, modelId, skeletonState);
	}

	mSkeletonStates[modelId].get().createAnimationState(animation);
}

void AnimationManager::terminate()
{
	mSkeletonStates.clear();
}

const std::vector<Matrix4>& AnimationManager::getBoneTransforms(ObjectId modelId) const
{
	return mSkeletonStates.at(modelId).get().getCurrentBoneTransforms();
}
