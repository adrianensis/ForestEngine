#ifndef ANIMATIONMANAGER_HPP
#define ANIMATIONMANAGER_HPP

#include "Core/Module.hpp"

class Model;
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
    const std::vector<Matrix4>& getBoneTransforms(Ptr<const Model> model) const;
    void terminate();

private:
    std::map<Ptr<const Model>, OwnerPtr<SkeletonState>> mSkeletonStates;
};

#endif