#include "Graphics/GPU/GPUMesh.hpp"

void GPUMesh::init(u32 vertexCount, u32 facesCount)
{
	mVertexCount = vertexCount;
	mFacesCount = facesCount;

    mGPUVertexInputBuffers = smDefaultGPUVertexInputBuffers;

	clear();
}

void GPUMesh::clear()
{
	mPositions.clear();
	mNormals.clear();
	mTextureCoordinates.clear();
	mColors.clear();
	mFaces.clear();
	mBonesVertexIDsData.clear();
	mBonesVertexWeightsData.clear();

	mPositions.reserve<Vector3>(mVertexCount);
	mTextureCoordinates.reserve<Vector2>(mVertexCount);
	mColors.reserve<Vector4>(mVertexCount);
	mNormals.reserve<Vector3>(mVertexCount);
	mFaces.reserve<Face>(mFacesCount);
	mBonesVertexIDsData.reserve<BoneVertexIDsData>(mVertexCount);
	mBonesVertexWeightsData.reserve<BoneVertexWeightsData>(mVertexCount);
}

void GPUMesh::setColor(const Vector4 &color)
{
    mColors.clear();
    mColors.resize<Vector4>(mVertexCount);
    mColors.fill(color);
}
