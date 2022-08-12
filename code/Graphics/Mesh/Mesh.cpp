#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Model/Model.hpp"
#include <algorithm>

void BoneVertexData::addBoneData(u32 id, f32 weight)
{
	FOR_RANGE(i, 0, smMaxBonesPerVertex)
	{
		if (mBoneWeights[i] == 0.0f)
		{
			mBoneIDs[i] = id;
			mBoneWeights[i] = weight;
			//printf("bone %d weight %f index %i\n", id, weight, i);
			return;
		}
	}

	// should never get here - more bones than we have space for
	ASSERT_MSG(false, "should never get here - more bones than we have space for");
}

Mesh::~Mesh()
{
}

void Mesh::init(u32 vertexCount, u32 facesCount)
{
	mVertexCount = vertexCount;
	mFacesCount = facesCount;

	clear();
}

void Mesh::addVertex(CR(Vector3) vector)
{
	mVertices.push_back(vector.x);
	mVertices.push_back(vector.y);
	mVertices.push_back(vector.z);
}

void Mesh::addNormal(CR(Vector3) vector)
{
	mNormals.push_back(vector.x);
	mNormals.push_back(vector.y);
	mNormals.push_back(vector.z);
}

void Mesh::addTexCoord(f32 u, f32 v)
{
	mTextureCoordinates.push_back(u);
	mTextureCoordinates.push_back(v);
}

void Mesh::addColor(f32 r, f32 g, f32 b, f32 a)
{
	mColors.push_back(r);
	mColors.push_back(g);
	mColors.push_back(b);
	mColors.push_back(a);
}

void Mesh::addFace(u32 v1, u32 v2, u32 v3)
{
	addFaceIndex(v1);
	addFaceIndex(v2);
	addFaceIndex(v3);
}

void Mesh::addBoneVertexData(CR(BoneVertexData) bone)
{
	mBonesVertexData.push_back(bone);
}

void Mesh::addFaceIndex(u32 i)
{
	mFaces.push_back(i);
}

void Mesh::addVertices(const std::vector<f32>& vec)
{
	mVertices.insert(mVertices.end(), vec.begin(), vec.end());
}

void Mesh::addNormals(const std::vector<f32>& vec)
{
	mNormals.insert(mNormals.end(), vec.begin(), vec.end());
}

void Mesh::addTextureCoordinates(const std::vector<f32>& vec)
{
	mTextureCoordinates.insert(mTextureCoordinates.end(), vec.begin(), vec.end());
}

void Mesh::addColors(const std::vector<f32>& vec)
{
	mColors.insert(mColors.end(), vec.begin(), vec.end());
}

void Mesh::addFaces(const std::vector<u32>& vec)
{
	mFaces.insert(mFaces.end(), vec.begin(), vec.end());
}

void Mesh::addBonesVertexData(const std::vector<BoneVertexData>& vec)
{
	mBonesVertexData.insert(mBonesVertexData.end(), vec.begin(), vec.end());
}

void Mesh::addBoneWeight(u32 vertexId, u32 id, f32 weight)
{
	mBonesVertexData[vertexId].addBoneData(id, weight);
}

void Mesh::copyVertices(Ptr<const Mesh> other)
{
	std::copy(other.get().getVertices().begin(), other.get().getVertices().end(), back_inserter(mVertices));
}

void Mesh::copyNormals(Ptr<const Mesh> other)
{
	std::copy(other.get().getNormals().begin(), other.get().getNormals().end(), back_inserter(mNormals));
}

void Mesh::copyTextureCoordinates(Ptr<const Mesh> other)
{
	std::copy(other.get().getTextureCoordinates().begin(), other.get().getTextureCoordinates().end(), back_inserter(mTextureCoordinates));
}

void Mesh::copyColors(Ptr<const Mesh> other)
{
	std::copy(other.get().getColors().begin(), other.get().getColors().end(), back_inserter(mColors));
}

void Mesh::copyFaces(Ptr<const Mesh> other)
{
	std::copy(other.get().getFaces().begin(), other.get().getFaces().end(), back_inserter(mFaces));
}

void Mesh::copyBones(Ptr<const Mesh> other)
{
	std::copy(other.get().getBonesVertexData().begin(), other.get().getBonesVertexData().end(), back_inserter(mBonesVertexData));
}

void Mesh::clear()
{
	mVertices.clear();
	mNormals.clear();
	mTextureCoordinates.clear();
	mColors.clear();
	mFaces.clear();
	mBonesVertexData.clear();
	//mBonesMapping.clear();

	//mBonesIndexCount = 0;

	mVertices.reserve(mVertexCount * smVertexPositionSize);
	mTextureCoordinates.reserve(mVertexCount * smVertexTexCoordSize);
	mColors.reserve(mVertexCount * smVertexColorSize);
	mNormals.reserve(mVertexCount * smVertexNormalSize);
	mFaces.reserve(mFacesCount * smFaceSize);
	mBonesVertexData.reserve(mVertexCount);

	std::fill(mColors.begin(), mColors.end(), 0);

	mMaterialPath.clear();
}

void Mesh::getBoneTransforms(float TimeInSeconds, std::vector<Matrix4>& Transforms) const
{
    test = 0;

    Matrix4 Identity;
	Identity.identity();

	const aiScene* m_pScene = mModel.get().getImporter().GetScene();

    aiAnimation* animation = m_pScene->mAnimations[0];

    float TicksPerSecond = (float)(animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, m_pScene->mAnimations[0]->mDuration);
    // we need to use the integral part of mDuration for the total length of the animation
    //double Duration = 0.0f;
    // float fraction = modf((float)animation->mDuration, &Duration);
    // float AnimationTimeTicks = fmod(TimeInTicks, Duration);

    Transforms.resize(mModel.get().getBonesIndexCount());

    FOR_ARRAY(it, Transforms)
    {
        Transforms[it] = Matrix4::getIdentity();
    }

    readNodeHierarchy(AnimationTime, m_pScene->mRootNode, Identity, Transforms);
}

void Mesh::readNodeHierarchy(float animationTimeTicks, const aiNode* pNode, const Matrix4& parentTransform, std::vector<Matrix4>& currentTransforms) const
{
    std::string NodeName(pNode->mName.data);

	const aiScene* m_pScene = mModel.get().getImporter().GetScene();
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
        CalcInterpolatedScaling(Scaling, animationTimeTicks, pNodeAnim);
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
        CalcInterpolatedRotation(RotationQ, animationTimeTicks, pNodeAnim);
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
        CalcInterpolatedPosition(Translation, animationTimeTicks, pNodeAnim);
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

    if (MAP_CONTAINS(mModel.get().getBonesMapping(), NodeName)) {
        u32 boneIndex = mModel.get().getBonesMapping().at(NodeName).mId;

	    Matrix4 globalInverseTransform = mModel.get().getGlobalInverseTransform();
		Matrix4 globalTransformationCopy(globalTransformation);
		globalTransformationCopy.mul(mModel.get().getBonesMapping().at(NodeName).mOffsetMatrix);
		globalInverseTransform.mul(globalTransformationCopy);
		
        //mBonesData[BoneIndex].FinalTransformation = globalInverseTransform;
        // mBonesData[BoneIndex].FinalTransformation = m_GlobalInverseTransform * globalTransformation * mBonesData[BoneIndex].mOffsetMatrix;
		currentTransforms[boneIndex] = globalInverseTransform;
    }

        Vector3 cubeTopLeft(0,0,0);
        cubeTopLeft = globalTransformation.mulVector(Vector4(cubeTopLeft, 1));
        RenderEngine::getInstance().drawCube(Cube(cubeTopLeft,Vector3(0.1f,0.1f,0.1f)), 1, true, Vector4(0.5f,0.5f,0,1));


        Vector3 parentPosition(0,0,0);
        parentPosition = parentTransform.mulVector(Vector4(parentPosition, 1));
        
        RenderEngine::getInstance().drawLine(Line(parentPosition, cubeTopLeft), 1, true, Vector4(0.5f,0.5f,0,1));

    for (u32 i = 0 ; i < pNode->mNumChildren ; i++)
    {
        readNodeHierarchy(animationTimeTicks, pNode->mChildren[i], globalTransformation, currentTransforms);
    }
}

const aiNodeAnim* Mesh::findNodeAnim(const aiAnimation* animation, const std::string& nodeName) const
{
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

u32 Mesh::FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
    for (u32 i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
        if (AnimationTimeTicks < t) {
            return i;
        }
    }

    return 0;
}

void Mesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    u32 PositionIndex = FindPosition(AnimationTimeTicks, pNodeAnim);
    u32 NextPositionIndex = PositionIndex + 1;
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
    if (t1 > AnimationTimeTicks) {
        Out = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    } else {
        float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
        float DeltaTime = t2 - t1;
        float Factor = (AnimationTimeTicks - t1) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
        const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }
}

u32 Mesh::FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (u32 i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
        if (AnimationTimeTicks < t) {
            return i;
        }
    }

    return 0;
}

void Mesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    u32 RotationIndex = FindRotation(AnimationTimeTicks, pNodeAnim);
    u32 NextRotationIndex = RotationIndex + 1;
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
    if (t1 > AnimationTimeTicks) {
        Out = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    } else {
        float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
        float DeltaTime = t2 - t1;
        float Factor = (AnimationTimeTicks - t1) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
        const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
        aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
        Out = StartRotationQ;
    }

    Out.Normalize();
}

u32 Mesh::FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (u32 i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
        if (AnimationTimeTicks < t) {
            return i;
        }
    }

    return 0;
}

void Mesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    u32 ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
    u32 NextScalingIndex = ScalingIndex + 1;
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
    if (t1 > AnimationTimeTicks) {
        Out = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    } else {
        float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
        float DeltaTime = t2 - t1;
        float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
        const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
    }
}