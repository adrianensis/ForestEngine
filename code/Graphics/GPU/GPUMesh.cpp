#include "Graphics/GPU/GPUMesh.hpp"

void GPUMesh::init(u32 vertexCount, u32 facesCount)
{
	mVertexCount = vertexCount;
	mFacesCount = facesCount;

	clear();
}

void GPUMesh::addBoneWeight(u32 vertexId, i32 id, f32 weight)
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
	CHECK_MSG(false, "should never get here - more bones than we have space for");
}

void GPUMesh::clear()
{
	clearPositions();
	clearNormals();
	clearTextureCoordinates();
	clearColors();
	clearFaces();
	clearBonesVertexIDsData();
	clearBonesVertexWeightsData();

	mPositions.reserve(mVertexCount);
	mTextureCoordinates.reserve(mVertexCount);
	mColors.reserve(mVertexCount);
	mNormals.reserve(mVertexCount);
	mFaces.reserve(mFacesCount);
	mBonesVertexIDsData.reserve(mVertexCount);
	mBonesVertexWeightsData.reserve(mVertexCount);
}

void GPUMesh::setColor(const Vector4 &color)
{
    mColors.clear();
    mColors.resize(mVertexCount);
    std::fill(mColors.begin(), mColors.end(), color);
}
