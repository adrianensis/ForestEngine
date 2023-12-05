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
	OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>::newObject();
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
OwnerPtr<Mesh> MeshPrimitives::createPrimitive<Cube>() const
{
	OwnerPtr<Mesh> mesh = OwnerPtr<Mesh>::newObject();
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

	Vector3 leftNormal(-1,0,0); // left
	Vector3 rightNormal(1,0,0); // right
	Vector3 frontNormal(0,0,1); // front
	Vector3 backNormal(0,0,-1); // back
	Vector3 topNormal(0,1,0); // top
	Vector3 bottomNormal(0,-1,0); // bottom

	mesh->addToPositions(bottomLeftFront);
	mesh->addToPositions(bottomRightFront);
	mesh->addToPositions(topRightFront);
	mesh->addToPositions(topLeftFront);
	
    mesh->addToNormals(frontNormal);
	mesh->addToNormals(frontNormal);
	mesh->addToNormals(frontNormal);
	mesh->addToNormals(frontNormal);

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
	
    mesh->addToNormals(leftNormal);
	mesh->addToNormals(leftNormal);
	mesh->addToNormals(leftNormal);
	mesh->addToNormals(leftNormal);

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
	
    mesh->addToNormals(backNormal);
	mesh->addToNormals(backNormal);
	mesh->addToNormals(backNormal);
	mesh->addToNormals(backNormal);

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
	
    mesh->addToNormals(rightNormal);
	mesh->addToNormals(rightNormal);
	mesh->addToNormals(rightNormal);
	mesh->addToNormals(rightNormal);

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
	
    mesh->addToNormals(topNormal);
	mesh->addToNormals(topNormal);
	mesh->addToNormals(topNormal);
	mesh->addToNormals(topNormal);

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
	
    mesh->addToNormals(bottomNormal);
	mesh->addToNormals(bottomNormal);
	mesh->addToNormals(bottomNormal);
	mesh->addToNormals(bottomNormal);

	mesh->addToTextureCoordinates(Vector2(0.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 0.0f));
	mesh->addToTextureCoordinates(Vector2(1.0f, 1.0f));
	mesh->addToTextureCoordinates(Vector2(0.0f, 1.0f));

	mesh->addToFaces(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->addToFaces(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	return mesh;
}

