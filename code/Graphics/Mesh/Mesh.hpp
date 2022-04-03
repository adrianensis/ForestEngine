#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Mesh/Mesh.hpp"
#include <algorithm>
#endif

class Mesh: public ObjectBase
{
    GENERATE_METADATA(Mesh)

private:
	std::vector<f32> mVertices;
	std::vector<f32> mNormals;
	std::vector<f32> mTextureCoordinates;
	std::vector<f32> mColors;
	std::vector<u16> mFaces;

	u32 mVertexCount = 0;
	u32 mFacesCount = 0;

public:
	inline static const u32 smVertexPositionSize = 3;
	inline static const u32 smVertexNormalSize = 3;
	inline static const u32 smVertexTexCoordSize = 2;
	inline static const u32 smVertexColorSize = 4;
	inline static const u32 smFaceSize = 3;

	CPP void init(u32 vertexCount, u32 facesCount)
	{
		mVertexCount = vertexCount;
		mFacesCount = facesCount;

		clear();
	}

	CPP void addVertex(const Vector3& vector)
	{
		mVertices.push_back(vector.x);
		mVertices.push_back(vector.y);
		mVertices.push_back(vector.z);
	}

	CPP void addNormal(const Vector3& vector)
	{
		mNormals.push_back(vector.x);
		mNormals.push_back(vector.y);
		mNormals.push_back(vector.z);
	}

	CPP void addTexCoord(f32 u, f32 v)
	{
		mTextureCoordinates.push_back(u);
		mTextureCoordinates.push_back(v);
	}

	CPP void addColor(f32 r, f32 g, f32 b, f32 a)
	{
		mColors.push_back(r);
		mColors.push_back(g);
		mColors.push_back(b);
		mColors.push_back(a);
	}

	CPP void addFace(u16 v1, u16 v2, u16 v3)
	{
		addFaceIndex(v1);
		addFaceIndex(v2);
		addFaceIndex(v3);
	}

	CPP void addFaceIndex(u16 i)
	{
		mFaces.push_back(i);
	}

	CPP void addVertices(const std::vector<f32> vec)
	{
		mVertices.insert(mVertices.end(), vec.begin(), vec.end());
	}

	CPP void addNormals(const std::vector<f32> vec)
	{
		mNormals.insert(mNormals.end(), vec.begin(), vec.end());
	}

	CPP void addTextureCoordinates(const std::vector<f32> vec)
	{
		mTextureCoordinates.insert(mTextureCoordinates.end(), vec.begin(), vec.end());
	}

	CPP void addColors(const std::vector<f32> vec)
	{
		mColors.insert(mColors.end(), vec.begin(), vec.end());
	}

	CPP void addFaces(const std::vector<u16> vec)
	{
		mFaces.insert(mFaces.end(), vec.begin(), vec.end());
	}

	CPP void copyVertices(Ptr<const Mesh> other)
	{
		std::copy(other.get().getVertices().begin(), other.get().getVertices().end(), back_inserter(mVertices));
	}

	CPP void copyNormals(Ptr<const Mesh> other)
	{
		std::copy(other.get().getNormals().begin(), other.get().getNormals().end(), back_inserter(mNormals));
	}

	CPP void copyTextureCoordinates(Ptr<const Mesh> other)
	{
		std::copy(other.get().getTextureCoordinates().begin(), other.get().getTextureCoordinates().end(), back_inserter(mTextureCoordinates));
	}

	CPP void copyColors(Ptr<const Mesh> other)
	{
		std::copy(other.get().getColors().begin(), other.get().getColors().end(), back_inserter(mColors));
	}

	CPP void copyFaces(Ptr<const Mesh> other)
	{
		std::copy(other.get().getFaces().begin(), other.get().getFaces().end(), back_inserter(mFaces));
	}

	CPP void clear()
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

	CRGET(Vertices)
	CRGET(Normals)
	CRGET(TextureCoordinates)
	CRGET(Colors)
	CRGET(Faces)
	GET(VertexCount)
	GET(FacesCount)
};