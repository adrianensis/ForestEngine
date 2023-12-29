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
    mColors.fill(color);
}
