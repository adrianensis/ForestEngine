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
