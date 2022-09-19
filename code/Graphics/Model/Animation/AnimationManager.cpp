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
	Ptr<const Model> model = animation.get().getModel();

	if(!MAP_CONTAINS(mSkeletonStates, model))
	{
		OwnerPtr<SkeletonState> skeletonState = OwnerPtr<SkeletonState>(NEW(SkeletonState));
		skeletonState.get().init(animation.get().getModel());

		MAP_INSERT(mSkeletonStates, model, skeletonState);
	}

	mSkeletonStates[model].get().createAnimationState(animation);
}

void AnimationManager::terminate()
{
	mSkeletonStates.clear();
}

const std::vector<Matrix4>& AnimationManager::getBoneTransforms(Ptr<const Model> model) const
{
	return mSkeletonStates.at(model).get().getCurrentBoneTransforms();
}
