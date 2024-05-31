#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Animation/Animation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/RenderState.hpp"

void AnimationManager::init()
{
}

void AnimationManager::update()
{
	PROFILER_CPU()

	FOR_MAP(it, mSkeletonStates)
	{
		it->second->update();
	}
}

void AnimationManager::createAnimationState(Ptr<const Animation> animation)
{
	Ptr<const Model> model = animation->mModel;

	if(!mSkeletonStates.contains(model))
	{
		mSkeletonStates.insert_or_assign(model, OwnerPtr<SkeletonState>::newObject());
		mSkeletonStates.at(model)->init(animation->mModel);
        
        GET_SYSTEM(RenderState).initSkeletonRenderState(mSkeletonStates[model]);
	}

	mSkeletonStates[model]->createAnimationState(animation);

}

void AnimationManager::terminate()
{
	mSkeletonStates.clear();
}

const std::vector<Matrix4>& AnimationManager::getBoneTransforms(Ptr<const Model> model) const
{
	return mSkeletonStates.at(model)->getCurrentBoneTransforms();
}
