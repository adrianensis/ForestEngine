#include "Graphics/Mesh.hpp"
#include "Core/Memory.hpp"

#include <algorithm>

void Mesh::init(u32 vertexCount, u32 facesCount)
{
	mVertexCount = vertexCount;
	mFacesCount = facesCount;

	clear();
}

void Mesh::addVertex(const Vector3& vector)
{
	mVertices.push_back(vector.x);
	mVertices.push_back(vector.y);
	mVertices.push_back(vector.z);
}

void Mesh::addNormal(const Vector3& vector)
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

void Mesh::addFace(u16 v1, u16 v2, u16 v3)
{
	addFaceIndex(v1);
	addFaceIndex(v2);
	addFaceIndex(v3);
}

void Mesh::addFaceIndex(u16 i)
{
	mFaces.push_back(i);
}

void Mesh::copyVertices(const Mesh* other)
{
	std::copy(other->getVertices().begin(), other->getVertices().end(), back_inserter(mVertices));
}

void Mesh::copyNormals(const Mesh* other)
{
	std::copy(other->getNormals().begin(), other->getNormals().end(), back_inserter(mNormals));
}

void Mesh::copyTextureCoordinates(const Mesh* other)
{
	std::copy(other->getTextureCoordinates().begin(), other->getTextureCoordinates().end(), back_inserter(mTextureCoordinates));
}

void Mesh::copyColors(const Mesh* other)
{
	std::copy(other->getColors().begin(), other->getColors().end(), back_inserter(mColors));
}

void Mesh::copyFaces(const Mesh* other)
{
	std::copy(other->getFaces().begin(), other->getFaces().end(), back_inserter(mFaces));
}

void Mesh::clear()
{
	mVertices.clear();
	mNormals.clear();
	mTextureCoordinates.clear();
	mColors.clear();
	mFaces.clear();

	mVertices.reserve(mVertexCount * smVertexPositionSize);
	mTextureCoordinates.reserve(mVertexCount * smVertexTexCoordSize);
	mColors.reserve(mVertexCount * smVertexColorSize);
	mNormals.reserve(mVertexCount * smVertexNormalSize);
	mFaces.reserve(mFacesCount * smFaceSize);

	std::fill(mColors.begin(), mColors.end(), 0);
}