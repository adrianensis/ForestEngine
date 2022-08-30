#include "Graphics/Mesh/MeshPrimitives.hpp"


void MeshPrimitives::init()
{

}

void MeshPrimitives::terminate()
{
	mPrimitivesMap.clear();
}

template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Rectangle>() const
{
	OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>(NEW(Mesh));
	mesh.get().init(4*1, 2*1);

	mesh.get().addPosition(Vector3(-0.5f, -0.5f, 0.0f)); // bottom left
	mesh.get().addPosition(Vector3(0.5f, -0.5f, 0.0f)); // bottom right
	mesh.get().addPosition(Vector3(0.5f, 0.5f, 0.0f)); // top right
	mesh.get().addPosition(Vector3(-0.5f, 0.5f, 0.0f)); // top left

	mesh.get().addTexCoord(Vector2(0.0f, 0.0f)); // bottom left
	mesh.get().addTexCoord(Vector2(1.0f, 0.0f)); // bottom right
	mesh.get().addTexCoord(Vector2(1.0f, 1.0f)); // top right
	mesh.get().addTexCoord(Vector2(0.0f, 1.0f)); // top left

	mesh.get().addFace(Face(0,1,2));
	mesh.get().addFace(Face(2,3,0));

	return mesh;
}

template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Cube>() const
{
	OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>(NEW(Mesh));
	mesh.get().init(4*6, 2*6);

	u32 elementOffsetIncrement = 4;
	u32 elementOffset = 0;

	Vector3 bottomLeftFront(-0.5f, -0.5f, 0.5f); // bottom left front
	Vector3 bottomRightFront(0.5f, -0.5f, 0.5f); // bottom right front
	Vector3 topLeftFront(-0.5f, 0.5f, 0.5f); // top right front
	Vector3 topRightFront(0.5f, 0.5f, 0.5f); // top left front

	Vector3 bottomLeftBack(-0.5f, -0.5f, -0.5f); // bottom left back
	Vector3 bottomRightBack(0.5f, -0.5f, -0.5f); // bottom right back
	Vector3 topLeftBack(-0.5f, 0.5f, -0.5f); // top right back
	Vector3 topRightBack(0.5f, 0.5f, -0.5f); // top left back

	mesh.get().addPosition(bottomLeftFront);
	mesh.get().addPosition(bottomRightFront);
	mesh.get().addPosition(topRightFront);
	mesh.get().addPosition(topLeftFront);

	mesh.get().addTexCoord(Vector2(0.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 1.0f));
	mesh.get().addTexCoord(Vector2(0.0f, 1.0f));

	mesh.get().addFace(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addFace(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh.get().addPosition(bottomLeftBack);
	mesh.get().addPosition(bottomLeftFront);
	mesh.get().addPosition(topLeftFront);
	mesh.get().addPosition(topLeftBack);

	mesh.get().addTexCoord(Vector2(0.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 1.0f));
	mesh.get().addTexCoord(Vector2(0.0f, 1.0f));

	mesh.get().addFace(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addFace(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh.get().addPosition(bottomRightBack);
	mesh.get().addPosition(bottomLeftBack);
	mesh.get().addPosition(topLeftBack);
	mesh.get().addPosition(topRightBack);

	mesh.get().addTexCoord(Vector2(0.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 1.0f));
	mesh.get().addTexCoord(Vector2(0.0f, 1.0f));

	mesh.get().addFace(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addFace(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh.get().addPosition(bottomRightFront);
	mesh.get().addPosition(bottomRightBack);
	mesh.get().addPosition(topRightBack);
	mesh.get().addPosition(topRightFront);

	mesh.get().addTexCoord(Vector2(0.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 1.0f));
	mesh.get().addTexCoord(Vector2(0.0f, 1.0f));

	mesh.get().addFace(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addFace(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh.get().addPosition(topLeftFront);
	mesh.get().addPosition(topRightFront);
	mesh.get().addPosition(topRightBack);
	mesh.get().addPosition(topLeftBack);

	mesh.get().addTexCoord(Vector2(0.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 1.0f));
	mesh.get().addTexCoord(Vector2(0.0f, 1.0f));

	mesh.get().addFace(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addFace(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh.get().addPosition(bottomLeftBack);
	mesh.get().addPosition(bottomRightBack);
	mesh.get().addPosition(bottomRightFront);
	mesh.get().addPosition(bottomLeftFront);

	mesh.get().addTexCoord(Vector2(0.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 0.0f));
	mesh.get().addTexCoord(Vector2(1.0f, 1.0f));
	mesh.get().addTexCoord(Vector2(0.0f, 1.0f));

	mesh.get().addFace(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addFace(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	return mesh;
}

