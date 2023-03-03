#pragma once

#include "Core/Module.hpp"

class Model;
class Animation;
class AnimationState;
class SkeletonState;

class AnimationManager: public EngineSystem
{
	GENERATE_METADATA(AnimationManager)

public:
    void init();
    void update();
    void createAnimationState(Ptr<const Animation> animation);
    const std::vector<Matrix4>& getBoneTransforms(Ptr<const Model> model) const;
    void terminate();

private:
    std::unordered_map<Ptr<const Model>, OwnerPtr<SkeletonState>> mSkeletonStates;
};
