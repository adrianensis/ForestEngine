#include "Graphics/Model/Animation/Animation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/RenderEngine.hpp"

#include "assimp/scene.h" // Output data structure

void Animation::init(u32 animationIndex, Ptr<const Model> model)
{
    mModel = model;
    mAnimationIndex = animationIndex;

    const aiScene* m_pScene = mModel.get().mImporter.GetScene();
    aiAnimation* aiAnimation = m_pScene->mAnimations[mAnimationIndex];

    mTicksPerSecond = (f32)(aiAnimation->mTicksPerSecond != 0 ? aiAnimation->mTicksPerSecond : 25.0f);
    mDurationInTicks = aiAnimation->mDuration;
    mDurationInSeconds = mDurationInTicks / mTicksPerSecond;
}

f32 Animation::calculateCurrentAnimationTime(f32 accumulatedTime) const
{
    f32 timeInTicks = accumulatedTime * mTicksPerSecond;
    f32 animationTime = fmod(timeInTicks, mDurationInTicks);
    return animationTime;
}

void AnimationState::init(Ptr<const Animation> animation)
{
    mAnimation = animation;
}

void AnimationState::update()
{
    mAnimationTime = mAnimation.get().calculateCurrentAnimationTime(mAccumulatedTime);

    f32 dt = Time::getInstance().getDeltaTimeSeconds();
    mAccumulatedTime += dt;

    // reset accumulatedTime to avoid overflow
    if(mAccumulatedTime > mAnimation.get().mDurationInSeconds)
    {
        mAccumulatedTime = 0;
    }
}

void SkeletonState::init(Ptr<const Model> model)
{
    mModel = model;
}

void SkeletonState::update()
{
	PROFILER_CPU()

    if(mCurrentAnimation)
    {
        mCurrentAnimation.get().update();

        getBoneTransforms(mCurrentBoneTransforms);
    }
}

void SkeletonState::createAnimationState(Ptr<const Animation> animation)
{
    ObjectId animationId = animation.get().getObjectId();

	if(!MAP_CONTAINS(mAnimationStates, animationId))
	{
		OwnerPtr<AnimationState> animationState = OwnerPtr<AnimationState>::newObject();
		animationState.get().init(animation);

		MAP_INSERT(mAnimationStates, animationId, animationState);
	}

    // TMP: grab the first anim state for testing
    if(! mCurrentAnimation)
    {
        mCurrentAnimation = mAnimationStates.at(animationId);
        
        // generate first bone transforms
        update();
    }
}

void SkeletonState::getBoneTransforms(std::vector<Matrix4>& Transforms) const
{
    getBoneTransformsFromCurrentAnimation(Transforms);
}

void SkeletonState::getBoneTransformsFromCurrentAnimation(std::vector<Matrix4>& Transforms) const
{
	PROFILER_CPU()

    Matrix4 Identity;
	Identity.identity();

	const aiScene* m_pScene = mCurrentAnimation.get().getAnimation().get().mModel.get().mImporter.GetScene();

    f32 animationTime = mCurrentAnimation.get().getAnimationTime();

    Transforms.resize(mCurrentAnimation.get().getAnimation().get().mModel.get().mBonesIndexCount);

    FOR_ARRAY(it, Transforms)
    {
        Transforms[it] = Matrix4::getIdentity();
    }

    readNodeHierarchy(animationTime, m_pScene->mRootNode, Identity, Transforms);
}

void SkeletonState::readNodeHierarchy(f32 animationTimeTicks, const aiNode* pNode, const Matrix4& parentTransform, std::vector<Matrix4>& currentTransforms) const
{
	PROFILER_CPU()

    std::string NodeName(pNode->mName.data);

	const aiScene* m_pScene = mCurrentAnimation.get().getAnimation().get().mModel.get().mImporter.GetScene();
    const aiAnimation* pAnimation = m_pScene->mAnimations[0];

    Matrix4 nodeTransformation;
	//nodeTransformation.init(&(pNode->mTransformation.a1));
    nodeTransformation.init(
        Vector4(pNode->mTransformation.a1, pNode->mTransformation.a2, pNode->mTransformation.a3, pNode->mTransformation.a4),
        Vector4(pNode->mTransformation.b1, pNode->mTransformation.b2, pNode->mTransformation.b3, pNode->mTransformation.b4),
        Vector4(pNode->mTransformation.c1, pNode->mTransformation.c2, pNode->mTransformation.c3, pNode->mTransformation.c4),
        Vector4(pNode->mTransformation.d1, pNode->mTransformation.d2, pNode->mTransformation.d3, pNode->mTransformation.d4)
    );
	// NOTE: transpose is needed to move from row-major to column-major
	//nodeTransformation.transpose();

    const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, NodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        calcInterpolatedScaling(Scaling, animationTimeTicks, pNodeAnim);
        // Matrix4 scalingM;
        // scalingM.scale(Vector3(Scaling.x, Scaling.y, Scaling.z));
        aiMatrix4x4 scaling_matr;
		aiMatrix4x4::Scaling(Scaling, scaling_matr);
        Matrix4 scalingM;
        scalingM.init(
            Vector4(scaling_matr.a1, scaling_matr.a2, scaling_matr.a3, scaling_matr.a4),
            Vector4(scaling_matr.b1, scaling_matr.b2, scaling_matr.b3, scaling_matr.b4),
            Vector4(scaling_matr.c1, scaling_matr.c2, scaling_matr.c3, scaling_matr.c4),
            Vector4(scaling_matr.d1, scaling_matr.d2, scaling_matr.d3, scaling_matr.d4)
        );

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        calcInterpolatedRotation(RotationQ, animationTimeTicks, pNodeAnim);
        aiMatrix4x4 rotationMatrix = aiMatrix4x4(RotationQ.GetMatrix());
        Matrix4 rotationM;
        rotationM.init(
            Vector4(rotationMatrix.a1, rotationMatrix.a2, rotationMatrix.a3, rotationMatrix.a4),
            Vector4(rotationMatrix.b1, rotationMatrix.b2, rotationMatrix.b3, rotationMatrix.b4),
            Vector4(rotationMatrix.c1, rotationMatrix.c2, rotationMatrix.c3, rotationMatrix.c4),
            Vector4(rotationMatrix.d1, rotationMatrix.d2, rotationMatrix.d3, rotationMatrix.d4)
        );

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        calcInterpolatedPosition(Translation, animationTimeTicks, pNodeAnim);
        aiMatrix4x4 translate_matr;
		aiMatrix4x4::Translation(Translation, translate_matr);
        Matrix4 translationM;
        translationM.init(
            Vector4(translate_matr.a1, translate_matr.a2, translate_matr.a3, translate_matr.a4),
            Vector4(translate_matr.b1, translate_matr.b2, translate_matr.b3, translate_matr.b4),
            Vector4(translate_matr.c1, translate_matr.c2, translate_matr.c3, translate_matr.c4),
            Vector4(translate_matr.d1, translate_matr.d2, translate_matr.d3, translate_matr.d4)
        );
        // Matrix4 translationM;
        // translationM.translation(Vector3(Translation.x, Translation.y, Translation.z));
 
        // Combine the above transformations
		rotationM.mul(scalingM);
		translationM.mul(rotationM);
        nodeTransformation = translationM;
    }

	Matrix4 globalTransformation(parentTransform);
    globalTransformation.mul(nodeTransformation);

    const auto& bonesMapping = mCurrentAnimation.get().getAnimation().get().mModel.get().mBonesMapping;
    if (MAP_CONTAINS(bonesMapping, NodeName)) {
        u32 boneIndex = bonesMapping.at(NodeName).mId;

	    Matrix4 globalInverseTransform = mCurrentAnimation.get().getAnimation().get().mModel.get().mGlobalInverseTransform;
		Matrix4 globalTransformationCopy(globalTransformation);
		globalTransformationCopy.mul(bonesMapping.at(NodeName).mOffsetMatrix);
		globalInverseTransform.mul(globalTransformationCopy);
		
        //mBonesData[BoneIndex].FinalTransformation = globalInverseTransform;
        // mBonesData[BoneIndex].FinalTransformation = m_GlobalInverseTransform * globalTransformation * mBonesData[BoneIndex].mOffsetMatrix;
		currentTransforms[boneIndex] = globalInverseTransform;
    }

    // Vector3 cubeTopLeft(0,0,0);
    // cubeTopLeft = globalTransformation.mulVector(Vector4(cubeTopLeft, 1));
    // GET_SYSTEM(RenderEngine).drawCube(Cube(cubeTopLeft,Vector3(0.1f,0.1f,0.1f)), 1, true, Vector4(0.5f,0.5f,0,1));


    // Vector3 parentPosition(0,0,0);
    // parentPosition = parentTransform.mulVector(Vector4(parentPosition, 1));
    
    // GET_SYSTEM(RenderEngine).drawLine(Line(parentPosition, cubeTopLeft), 1, true, Vector4(0.5f,0.5f,0,1));

    // GET_SYSTEM(RenderEngine).drawCube(Cube(Vector3(0,0,0),Vector3(1,1,1)), 1, true, Vector4(1.0f,0.5f,0,1));

    for (u32 i = 0 ; i < pNode->mNumChildren ; i++)
    {
        readNodeHierarchy(animationTimeTicks, pNode->mChildren[i], globalTransformation, currentTransforms);
    }
}

const aiNodeAnim* SkeletonState::findNodeAnim(const aiAnimation* animation, const std::string& nodeName) const
{
	PROFILER_CPU()

    for (u32 i = 0 ; i < animation->mNumChannels ; i++)
    {
        const aiNodeAnim* pNodeAnim = animation->mChannels[i];
        if (std::string(pNodeAnim->mNodeName.data) == nodeName)
        {
            return pNodeAnim;
        }
    }

    return nullptr;
}

u32 SkeletonState::findPosition(f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
	PROFILER_CPU()

    for (u32 i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        f32 t = (f32)pNodeAnim->mPositionKeys[i + 1].mTime;
        if (AnimationTimeTicks < t) {
            return i;
        }
    }

    return 0;
}

void SkeletonState::calcInterpolatedPosition(aiVector3D& Out, f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
	PROFILER_CPU()

    // we need at least two values to interpolate...
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    u32 PositionIndex = findPosition(AnimationTimeTicks, pNodeAnim);
    u32 NextPositionIndex = PositionIndex + 1;
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    f32 t1 = (f32)pNodeAnim->mPositionKeys[PositionIndex].mTime;
    if (t1 > AnimationTimeTicks) {
        Out = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    } else {
        f32 t2 = (f32)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
        f32 DeltaTime = t2 - t1;
        f32 Factor = (AnimationTimeTicks - t1) / DeltaTime;
        assert(Factor >= 0.0f and Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
        const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }
}


u32 SkeletonState::findRotation(f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
	PROFILER_CPU()

    assert(pNodeAnim->mNumRotationKeys > 0);

    for (u32 i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        f32 t = (f32)pNodeAnim->mRotationKeys[i + 1].mTime;
        if (AnimationTimeTicks < t) {
            return i;
        }
    }

    return 0;
}

void SkeletonState::calcInterpolatedRotation(aiQuaternion& Out, f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
	PROFILER_CPU()

    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    u32 RotationIndex = findRotation(AnimationTimeTicks, pNodeAnim);
    u32 NextRotationIndex = RotationIndex + 1;
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    f32 t1 = (f32)pNodeAnim->mRotationKeys[RotationIndex].mTime;
    if (t1 > AnimationTimeTicks) {
        Out = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    } else {
        f32 t2 = (f32)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
        f32 DeltaTime = t2 - t1;
        f32 Factor = (AnimationTimeTicks - t1) / DeltaTime;
        assert(Factor >= 0.0f and Factor <= 1.0f);
        const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
        const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
        aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
        Out = StartRotationQ;
    }

    Out.Normalize();
}

u32 SkeletonState::findScaling(f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
	PROFILER_CPU()

    assert(pNodeAnim->mNumScalingKeys > 0);

    for (u32 i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        f32 t = (f32)pNodeAnim->mScalingKeys[i + 1].mTime;
        if (AnimationTimeTicks < t) {
            return i;
        }
    }

    return 0;
}

void SkeletonState::calcInterpolatedScaling(aiVector3D& Out, f32 AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
	PROFILER_CPU()

    // we need at least two values to interpolate...
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    u32 ScalingIndex = findScaling(AnimationTimeTicks, pNodeAnim);
    u32 NextScalingIndex = ScalingIndex + 1;
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    f32 t1 = (f32)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
    if (t1 > AnimationTimeTicks) {
        Out = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    } else {
        f32 t2 = (f32)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
        f32 DeltaTime = t2 - t1;
        f32 Factor = (AnimationTimeTicks - (f32)t1) / DeltaTime;
        assert(Factor >= 0.0f and Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
        const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }
}