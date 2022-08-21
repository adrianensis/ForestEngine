#ifndef ANIMATIONMANAGER_HPP
#define ANIMATIONMANAGER_HPP

#include "Core/Module.hpp"

class Animation;
class AnimationState;
class SkeletonState;

class AnimationManager: public ObjectBase, public Singleton<AnimationManager>
{
	GENERATE_METADATA(AnimationManager)

public:
    void init();
    void update();
    void createAnimationState(Ptr<const Animation> animation);
    const std::vector<Matrix4>& getBoneTransforms(ObjectId modelId) const;
    void terminate();

private:
    std::map<ObjectId, OwnerPtr<SkeletonState>> mSkeletonStates;
};

#endif