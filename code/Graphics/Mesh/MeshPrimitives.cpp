#include "Graphics/Mesh/MeshPrimitives.hpp"


void MeshPrimitives::init()
{

}

void MeshPrimitives::terminate()
{
	mPrimitivesMap.clear();
}

template <>
SharedPtr<Mesh> MeshPrimitives::createPrimitive<Rectangle>() const
{
	SharedPtr<Mesh> mesh = SharedPtr<Mesh>::newObject();
	mesh->init(4*1, 2*1);

	mesh->addToPositions(Vector3(-0.5f, -0.5f, 0.0f)); // bottom left
	mesh->addToPositions(Vector3(0.5f, -0.5f, 0.0f)); // bottom right
	mesh->addToPositions(Vector3(0.5f, 0.5f, 0.0f)); // top right
	mesh->addToPositions(Vector3(-0.5f, 0.5f, 0.0f)); // top left

	mesh->addToTextureCoordinates(Vector2(0.0f, 0.0f)); // bottom left
	mesh->addToTextureCoordinates(Vector2(1.0f, 0.0f)); // bottom right
	mesh->addToTextureCoordinates(Vector2(1.0f, 1.0f)); // top right
	mesh->addToTextureCoordinates(Vector2(0.0f, 1.0f)); // top left

	mesh->addToFaces(Face(0,1,2));
	mesh->addToFaces(Face(2,3,0));

	return mesh;
}

template <>
SharedPtr<Mesh> MeshPrimitives::createPrimitive<Cube>() const
{
	SharedPtr<Mesh> mesh = SharedPtr<Mesh>::newObject();
	mesh->init(4*6, 2*6);

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

	mesh->addToPositions(bottomLeftFront);
	mesh->addToPositions(bottomRightFront);
	mesh->addToPositions(topRightFront);
	mesh->addToPositions(topLeftFront);

	mesh->addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh->addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh->addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->addToPositions(bottomLeftBack);
	mesh->addToPositions(bottomLeftFront);
	mesh->addToPositions(topLeftFront);
	mesh->addToPositions(topLeftBack);

	mesh->addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh->addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh->addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->addToPositions(bottomRightBack);
	mesh->addToPositions(bottomLeftBack);
	mesh->addToPositions(topLeftBack);
	mesh->addToPositions(topRightBack);

	mesh->addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh->addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh->addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->addToPositions(bottomRightFront);
	mesh->addToPositions(bottomRightBack);
	mesh->addToPositions(topRightBack);
	mesh->addToPositions(topRightFront);

	mesh->addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh->addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh->addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->addToPositions(topLeftFront);
	mesh->addToPositions(topRightFront);
	mesh->addToPositions(topRightBack);
	mesh->addToPositions(topLeftBack);

	mesh->addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh->addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh->addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->addToPositions(bottomLeftBack);
	mesh->addToPositions(bottomRightBack);
	mesh->addToPositions(bottomRightFront);
	mesh->addToPositions(bottomLeftFront);

	mesh->addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh->addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh->addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	return mesh;
}

