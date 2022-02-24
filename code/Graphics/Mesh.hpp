#pragma once

#include "Core/Module.hpp"

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

	void init(u32 vertexCount, u32 facesCount);
	void addVertex(const Vector3& vec);
	void addNormal(const Vector3& vec);
	void addTexCoord(f32 u, f32 v);
	void addColor(f32 r, f32 g, f32 b, f32 a);
	void addFace(u16 v1, u16 v2, u16 v3);
	void addFaceIndex(u16 v1);

	void copyVertices(const Mesh* other);
	void copyNormals(const Mesh* other);
	void copyTextureCoordinates(const Mesh* other);
	void copyColors(const Mesh* other);
	void copyFaces(const Mesh* other);

	void clear();

	GET_RC(Vertices)
	GET_RC(Normals)
	GET_RC(TextureCoordinates)
	GET_RC(Colors)
	GET_RC(Faces)
	GET(VertexCount)
	GET(FacesCount)
};