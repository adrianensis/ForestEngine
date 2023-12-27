#include "Graphics/GPU/GPUMesh.hpp"

void GPUMesh::init(u32 vertexCount, u32 facesCount)
{
	mVertexCount = vertexCount;
	mFacesCount = facesCount;

	clear();
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
