#include "Graphics/Model/SkeletalAnimation/SkeletalAnimationManager.hpp"
#include "Graphics/Model/SkeletalAnimation/SkeletalAnimation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/RenderState.hpp"

void SkeletalAnimationManager::init()
{
}

void SkeletalAnimationManager::update()
{
	PROFILER_CPU()

	FOR_MAP(it, mSkeletonStates)
	{
		it->second->update();
	}
}

void SkeletalAnimationManager::createSkeletalAnimationState(Ptr<const SkeletalAnimation> animation)
{
	Ptr<const Model> model = animation->mModel;

	if(!mSkeletonStates.contains(model))
	{
		mSkeletonStates.insert_or_assign(model, OwnerPtr<SkeletonState>::newObject());
		mSkeletonStates.at(model)->init(animation->mModel);
        
        GET_SYSTEM(RenderState).initSkeletonRenderState(mSkeletonStates[model]);
	}

	mSkeletonStates[model]->createSkeletalAnimationState(animation);

}

void SkeletalAnimationManager::terminate()
{
	mSkeletonStates.clear();
}

const std::vector<Matrix4>& SkeletalAnimationManager::getBoneTransforms(Ptr<const Model> model) const
{
	return mSkeletonStates.at(model)->getCurrentBoneTransforms();
}
