#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "Core/Module.hpp"
#include "assimp/Importer.hpp"

class aiNode;
class aiNodeAnim;
class aiAnimation;

class Model;

class Animation: public ObjectBase
{
    GENERATE_METADATA(Animation)

public:
    void init(u32 animationIndex, Ptr<const Model> model);

    f32 calculateCurrentAnimationTime(f32 accumulatedTime) const;

private:
    Ptr<const Model> mModel;
    i32 mAnimationIndex = -1;
    f32 mTicksPerSecond = 0;
    f32 mDurationInTicks = 0;
    f32 mDurationInSeconds = 0;

public:
    CGET(Model)
    GET(AnimationIndex)
    GET(DurationInTicks)
    GET(DurationInSeconds)
};

class AnimationState: public ObjectBase
{
    GENERATE_METADATA(AnimationState)

public:
    void init(Ptr<const Animation> animation);
    void update();

private:
    Ptr<const Animation> mAnimation;
    f32 mAccumulatedTime = 0;
    f32 mAnimationTime = 0;

public:
    CGET(Animation)
    GET(AnimationTime)
};

class SkeletonState: public ObjectBase
{
    GENERATE_METADATA(SkeletonState)

public:
    void init(Ptr<const Model> model);
    void createAnimationState(Ptr<const Animation> animation);
    void update();

private:
    void getBoneTransforms(std::vector<Matrix4>& Transforms) const;
    void getBoneTransformsFromCurrentAnimation(std::vector<Matrix4>& Transforms) const;

    const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string& nodeName) const;
    void readNodeHierarchy(f32 animationTimeTicks, const aiNode* pNode, const Matrix4& parentTransform, std::vector<Matrix4>& currentTransforms) const;
    u32 findPosition(f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;
    void calcInterpolatedPosition(aiVector3D& Out, f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;
    u32 findRotation(f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;
    void calcInterpolatedRotation(aiQuaternion& Out, f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;
    u32 findScaling(f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;
    void calcInterpolatedScaling(aiVector3D& Out, f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;

private:
    Ptr<const Model> mModel;
    std::map<ObjectId, OwnerPtr<AnimationState>> mAnimationStates;
    Ptr<AnimationState> mCurrentAnimation;
    std::vector<Matrix4> mCurrentBoneTransforms;

public:
    CRGET(CurrentBoneTransforms)
};

#endif