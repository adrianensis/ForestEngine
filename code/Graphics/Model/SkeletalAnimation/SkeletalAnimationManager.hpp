#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Graphics/Model/SkeletalAnimation/SkeletalAnimation.hpp"

class Model;

class SkeletalAnimationManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void update() override;
    void createSkeletalAnimationState(Ptr<const SkeletalAnimation> animation);
    const std::vector<Matrix4>& getBoneTransforms(Ptr<const Model> model) const;

private:
    std::unordered_map<Ptr<const Model>, OwnerPtr<SkeletonState>> mSkeletonStates;
public:
    CRGET(SkeletonStates)
};
REGISTER_CLASS(SkeletalAnimationManager);
