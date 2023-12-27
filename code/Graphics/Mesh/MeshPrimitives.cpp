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

	mesh->mPositions.pushBack(Vector3(-0.5f, -0.5f, 0.0f)); // bottom left
	mesh->mPositions.pushBack(Vector3(0.5f, -0.5f, 0.0f)); // bottom right
	mesh->mPositions.pushBack(Vector3(0.5f, 0.5f, 0.0f)); // top right
	mesh->mPositions.pushBack(Vector3(-0.5f, 0.5f, 0.0f)); // top left

	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 0.0f)); // bottom left
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 0.0f)); // bottom right
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 1.0f)); // top right
	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 1.0f)); // top left

	mesh->mFaces.pushBack(Face(0,1,2));
	mesh->mFaces.pushBack(Face(2,3,0));

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

	mesh->mPositions.pushBack(bottomLeftFront);
	mesh->mPositions.pushBack(bottomRightFront);
	mesh->mPositions.pushBack(topRightFront);
	mesh->mPositions.pushBack(topLeftFront);
	
    mesh->mNormals.pushBack(frontNormal);
	mesh->mNormals.pushBack(frontNormal);
	mesh->mNormals.pushBack(frontNormal);
	mesh->mNormals.pushBack(frontNormal);

	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 1.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 1.0f));

	mesh->mFaces.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mFaces.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mPositions.pushBack(bottomLeftBack);
	mesh->mPositions.pushBack(bottomLeftFront);
	mesh->mPositions.pushBack(topLeftFront);
	mesh->mPositions.pushBack(topLeftBack);
	
    mesh->mNormals.pushBack(leftNormal);
	mesh->mNormals.pushBack(leftNormal);
	mesh->mNormals.pushBack(leftNormal);
	mesh->mNormals.pushBack(leftNormal);

	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 1.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 1.0f));

	mesh->mFaces.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mFaces.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mPositions.pushBack(bottomRightBack);
	mesh->mPositions.pushBack(bottomLeftBack);
	mesh->mPositions.pushBack(topLeftBack);
	mesh->mPositions.pushBack(topRightBack);
	
    mesh->mNormals.pushBack(backNormal);
	mesh->mNormals.pushBack(backNormal);
	mesh->mNormals.pushBack(backNormal);
	mesh->mNormals.pushBack(backNormal);

	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 1.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 1.0f));

	mesh->mFaces.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mFaces.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mPositions.pushBack(bottomRightFront);
	mesh->mPositions.pushBack(bottomRightBack);
	mesh->mPositions.pushBack(topRightBack);
	mesh->mPositions.pushBack(topRightFront);
	
    mesh->mNormals.pushBack(rightNormal);
	mesh->mNormals.pushBack(rightNormal);
	mesh->mNormals.pushBack(rightNormal);
	mesh->mNormals.pushBack(rightNormal);

	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 1.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 1.0f));

	mesh->mFaces.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mFaces.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mPositions.pushBack(topLeftFront);
	mesh->mPositions.pushBack(topRightFront);
	mesh->mPositions.pushBack(topRightBack);
	mesh->mPositions.pushBack(topLeftBack);
	
    mesh->mNormals.pushBack(topNormal);
	mesh->mNormals.pushBack(topNormal);
	mesh->mNormals.pushBack(topNormal);
	mesh->mNormals.pushBack(topNormal);

	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 1.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 1.0f));

	mesh->mFaces.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mFaces.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	mesh->mPositions.pushBack(bottomLeftBack);
	mesh->mPositions.pushBack(bottomRightBack);
	mesh->mPositions.pushBack(bottomRightFront);
	mesh->mPositions.pushBack(bottomLeftFront);
	
    mesh->mNormals.pushBack(bottomNormal);
	mesh->mNormals.pushBack(bottomNormal);
	mesh->mNormals.pushBack(bottomNormal);
	mesh->mNormals.pushBack(bottomNormal);

	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 0.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(1.0f, 1.0f));
	mesh->mTextureCoordinates.pushBack(Vector2(0.0f, 1.0f));

	mesh->mFaces.pushBack(Face(0+elementOffset,1+elementOffset,2+elementOffset));
	mesh->mFaces.pushBack(Face(2+elementOffset,3+elementOffset,0+elementOffset));

	elementOffset += elementOffsetIncrement;

	return mesh;
}

