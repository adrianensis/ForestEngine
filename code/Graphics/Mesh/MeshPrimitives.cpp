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
	OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>(Memory::newObject<Mesh>());
	mesh.get().init(4*1, 2*1);

	mesh.get().addToPositions(Vector3(-0.5f, -0.5f, 0.0f)); // bottom left
	mesh.get().addToPositions(Vector3(0.5f, -0.5f, 0.0f)); // bottom right
	mesh.get().addToPositions(Vector3(0.5f, 0.5f, 0.0f)); // top right
	mesh.get().addToPositions(Vector3(-0.5f, 0.5f, 0.0f)); // top left

	mesh.get().addToTextureCoordinates(Vector2(0.0f, 0.0f)); // bottom left
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 0.0f)); // bottom right
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 1.0f)); // top right
	mesh.get().addToTextureCoordinates(Vector2(0.0f, 1.0f)); // top left

	mesh.get().addToFaces(Face(0,1,2));
	mesh.get().addToFaces(Face(2,3,0));

	return mesh;
}

template <>
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Cube>() const
{
	OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>(Memory::newObject<Mesh>());
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

	mesh.get().addToPositions(bottomLeftFront);
	mesh.get().addToPositions(bottomRightFront);
	mesh.get().addToPositions(topRightFront);
	mesh.get().addToPositions(topLeftFront);

	mesh.get().addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh.get().addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh.get().addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh.get().addToPositions(bottomLeftBack);
	mesh.get().addToPositions(bottomLeftFront);
	mesh.get().addToPositions(topLeftFront);
	mesh.get().addToPositions(topLeftBack);

	mesh.get().addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh.get().addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh.get().addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh.get().addToPositions(bottomRightBack);
	mesh.get().addToPositions(bottomLeftBack);
	mesh.get().addToPositions(topLeftBack);
	mesh.get().addToPositions(topRightBack);

	mesh.get().addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh.get().addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh.get().addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh.get().addToPositions(bottomRightFront);
	mesh.get().addToPositions(bottomRightBack);
	mesh.get().addToPositions(topRightBack);
	mesh.get().addToPositions(topRightFront);

	mesh.get().addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh.get().addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh.get().addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh.get().addToPositions(topLeftFront);
	mesh.get().addToPositions(topRightFront);
	mesh.get().addToPositions(topRightBack);
	mesh.get().addToPositions(topLeftBack);

	mesh.get().addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh.get().addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh.get().addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh.get().addToPositions(bottomLeftBack);
	mesh.get().addToPositions(bottomRightBack);
	mesh.get().addToPositions(bottomRightFront);
	mesh.get().addToPositions(bottomLeftFront);

	mesh.get().addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh.get().addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh.get().addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh.get().addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh.get().addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	return mesh;
}

