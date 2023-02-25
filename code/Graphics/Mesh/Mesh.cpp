#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Model/Model.hpp"

void Mesh::init(u32 vertexCount, u32 facesCount)
{
	mVertexCount = vertexCount;
	mFacesCount = facesCount;

	clear();
}

void Mesh::addBoneWeight(u32 vertexId, i32 id, f32 weight)
{
	BoneVertexIDsData& boneVertexIDsData = mBonesVertexIDsData[vertexId]; //.setBoneWeight(id, weight);
	BoneVertexWeightsData& boneVertexWeightsData = mBonesVertexWeightsData[vertexId]; //.setBoneWeight(id, weight);

    FOR_RANGE(i, 0, smMaxBonesPerVertex)
	{
		if (boneVertexIDsData.mBonesIDs[i] == -1)
		{
			boneVertexIDsData.mBonesIDs[i] = id;
			boneVertexWeightsData.mBonesWeights[i] = weight;
			//printf("bone %d weight %f index %i\n", id, weight, i);
			return;
		}
	}

	// should never get here - more bones than we have space for
	//ASSERT_MSG(false, "should never get here - more bones than we have space for");
}

void Mesh::clear()
{
	mPositions.clear();
	mNormals.clear();
	mTextureCoordinates.clear();
	mColors.clear();
	mFaces.clear();
	mBonesVertexIDsData.clear();
	mBonesVertexWeightsData.clear();

	mPositions.reserve(mVertexCount);
	mTextureCoordinates.reserve(mVertexCount);
	mColors.reserve(mVertexCount);
	mNormals.reserve(mVertexCount);
	mFaces.reserve(mFacesCount);
	mBonesVertexIDsData.reserve(mVertexCount);
	mBonesVertexWeightsData.reserve(mVertexCount);

    mMaterial.invalidate();
}

void Mesh::setColor(const Vector4 &color)
{
    mColors.clear();
    mColors.resize(mVertexCount);
    std::fill(mColors.begin(), mColors.end(), color);
}
