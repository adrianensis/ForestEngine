#include "Graphics/Mesh/Mesh.hpp"
#include <algorithm>
#include "assimp/scene.h" // Output data structure

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

void Mesh::addVertices(const std::vector<f32> vec)
{
	mVertices.insert(mVertices.end(), vec.begin(), vec.end());
}

void Mesh::addNormals(const std::vector<f32> vec)
{
	mNormals.insert(mNormals.end(), vec.begin(), vec.end());
}

void Mesh::addTextureCoordinates(const std::vector<f32> vec)
{
	mTextureCoordinates.insert(mTextureCoordinates.end(), vec.begin(), vec.end());
}

void Mesh::addColors(const std::vector<f32> vec)
{
	mColors.insert(mColors.end(), vec.begin(), vec.end());
}

void Mesh::addFaces(const std::vector<u32> vec)
{
	mFaces.insert(mFaces.end(), vec.begin(), vec.end());
}

void Mesh::addBonesVertexData(const std::vector<BoneVertexData> vec)
{
	mBonesVertexData.insert(mBonesVertexData.end(), vec.begin(), vec.end());
}

u32 Mesh::registerBone(CR(std::string) name)
{
	u32 boneIndex = mBonesIndexCount;
	mBonesIndexCount++;
	BoneData bi;
	mBonesData.push_back(bi);
	
	if(!MAP_CONTAINS(mBonesNameIndexMap, name))
	{
		MAP_INSERT(mBonesNameIndexMap, name, boneIndex);
	}

	mBonesNameIndexMap[name] = boneIndex;

	return boneIndex;
}

bool Mesh::isBoneRegistered(CR(std::string) name) const
{
	return MAP_CONTAINS(mBonesNameIndexMap, name);
}

u32 Mesh::getBoneID(CR(std::string) name) const
{
	return mBonesNameIndexMap.at(name);
}

void Mesh::setBoneOffsetMatrix(u32 id, CR(Matrix4) offsetMatrix)
{
	mBonesData[id].mOffsetMatrix = offsetMatrix;
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
	mBonesData.clear();
	mBonesNameIndexMap.clear();

	mBonesIndexCount = 0;

	mVertices.reserve(mVertexCount * smVertexPositionSize);
	mTextureCoordinates.reserve(mVertexCount * smVertexTexCoordSize);
	mColors.reserve(mVertexCount * smVertexColorSize);
	mNormals.reserve(mVertexCount * smVertexNormalSize);
	mFaces.reserve(mFacesCount * smFaceSize);
	mBonesVertexData.reserve(mVertexCount);

	std::fill(mColors.begin(), mColors.end(), 0);

	mMaterialPath.clear();
}

// void Mesh::GetBoneTransforms(float TimeInSeconds, std::vector<Matrix4>& Transforms)
// {
//     Matrix4 Identity;
//     // Identity.InitIdentity();

//     // float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
//     // float TimeInTicks = TimeInSeconds * TicksPerSecond;
//     // // we need to use the integral part of mDuration for the total length of the animation
//     // float Duration = 0.0f;
//     // float fraction = modf((float)m_pScene->mAnimations[0]->mDuration, &Duration);
//     // float AnimationTimeTicks = fmod(TimeInTicks, Duration);

//     readNodeHierarchy(AnimationTimeTicks, m_pScene->mRootNode, Identity);
//     Transforms.resize(m_BoneInfo.size());

//     for (uint i = 0 ; i < m_BoneInfo.size() ; i++) {
//         Transforms[i] = m_BoneInfo[i].FinalTransformation;
//     }
// }

// void Mesh::readNodeHierarchy(float animationTimeTicks, const aiNode* pNode, const Matrix4& parentTransform)
// {
//     std::string NodeName(pNode->mName.data);

//     const aiAnimation* pAnimation = m_pScene->mAnimations[0];

//     Matrix4 NodeTransformation(pNode->mTransformation);

//     const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, NodeName);

//     if (pNodeAnim) {
//         // Interpolate scaling and generate scaling transformation matrix
//         aiVector3D Scaling;
//         CalcInterpolatedScaling(Scaling, animationTimeTicks, pNodeAnim);
//         Matrix4 ScalingM;
//         ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

//         // Interpolate rotation and generate rotation transformation matrix
//         aiQuaternion RotationQ;
//         CalcInterpolatedRotation(RotationQ, animationTimeTicks, pNodeAnim);
//         Matrix4 RotationM = Matrix4(RotationQ.GetMatrix());

//         // Interpolate translation and generate translation transformation matrix
//         aiVector3D Translation;
//         CalcInterpolatedPosition(Translation, animationTimeTicks, pNodeAnim);
//         Matrix4 TranslationM;
//         TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

//         // Combine the above transformations
//         NodeTransformation = TranslationM * RotationM * ScalingM;
//     }

//     Matrix4 GlobalTransformation = parentTransform * NodeTransformation;

//     if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end()) {
//         uint BoneIndex = m_BoneNameToIndexMap[NodeName];
//         m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
//     }

//     for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
//         readNodeHierarchy(animationTimeTicks, pNode->mChildren[i], GlobalTransformation);
//     }
// }

// const aiNodeAnim* Mesh::findNodeAnim(const aiAnimation* animation, const std::string& nodeName)
// {
//     for (uint i = 0 ; i < animation->mNumChannels ; i++)
//     {
//         const aiNodeAnim* pNodeAnim = animation->mChannels[i];

//         if (std::string(pNodeAnim->mNodeName.data) == nodeName)
//         {
//             return pNodeAnim;
//         }
//     }

//     return NULL;
// }