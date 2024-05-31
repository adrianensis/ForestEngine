#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Graphics/Model/Animation/Animation.hpp"

class Model;

class AnimationManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void update() override;
    void createAnimationState(Ptr<const Animation> animation);
    const std::vector<Matrix4>& getBoneTransforms(Ptr<const Model> model) const;

private:
    std::unordered_map<Ptr<const Model>, OwnerPtr<SkeletonState>> mSkeletonStates;
public:
    CRGET(SkeletonStates)
};
REGISTER_CLASS(AnimationManager);
