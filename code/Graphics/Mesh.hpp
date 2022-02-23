#pragma once

#include "Core/Module.hpp"

class Mesh: public ObjectBase
{
    GENERATE_METADATA(Mesh)
	PRI std::vector<f32> mVertices;
	PRI std::vector<f32> mNormals;
	PRI std::vector<f32> mTextureCoordinates;
	PRI std::vector<f32> mColors;
	PRI std::vector<u16> mFaces;

	PRI u32 mVertexCount = 0;
	PRI u32 mFacesCount = 0;

PRI
	static Mesh *smRectangle;

public:
	static const u32 smVertexPositionSize = 3;
	static const u32 smVertexNormalSize = 3;
	static const u32 smVertexTexCoordSize = 2;
	static const u32 smVertexColorSize = 4;
	static const u32 smFaceSize = 3;

	void init(u32 vertexCount, u32 facesCount);
	Mesh *addVertex(const Vector3& vec);
	Mesh *addNormal(const Vector3& vec);
	Mesh *addTexCoord(f32 u, f32 v);
	Mesh *addColor(f32 r, f32 g, f32 b, f32 a);
	Mesh *addFace(u16 v1, u16 v2, u16 v3);
	Mesh *addFaceIndex(u16 v1);

	Mesh *copyVertices(const Mesh* other);
	Mesh *copyNormals(const Mesh* other);
	Mesh *copyTextureCoordinates(const Mesh* other);
	Mesh *copyColors(const Mesh* other);
	Mesh *copyFaces(const Mesh* other);

	void clear();

	GET_RC(Vertices)
	GET_RC(Normals)
	GET_RC(TextureCoordinates)
	GET_RC(Colors)
	GET_RC(Faces)
	GET(VertexCount)
	GET(FacesCount)
};