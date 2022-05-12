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

mesh.get().addVertex(Vector3(-0.5f, -0.5f, 0.0f)); // bottom left
mesh.get().addVertex(Vector3(0.5f, -0.5f, 0.0f)); // bottom right
mesh.get().addVertex(Vector3(0.5f, 0.5f, 0.0f)); // top right
mesh.get().addVertex(Vector3(-0.5f, 0.5f, 0.0f)); // top left

mesh.get().addTexCoord(0.0f, 0.0f); // bottom left
mesh.get().addTexCoord(1.0f, 0.0f); // bottom right
mesh.get().addTexCoord(1.0f, 1.0f); // top right
mesh.get().addTexCoord(0.0f, 1.0f); // top left

mesh.get().addFace(0,1,2);
mesh.get().addFace(2,3,0);

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

mesh.get().addVertex(bottomLeftFront);
mesh.get().addVertex(bottomRightFront);
mesh.get().addVertex(topRightFront);
mesh.get().addVertex(topLeftFront);

mesh.get().addTexCoord(0.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 1.0f);
mesh.get().addTexCoord(0.0f, 1.0f);

mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

elementOffset += elementOffsetIncrement;

mesh.get().addVertex(bottomLeftBack);
mesh.get().addVertex(bottomLeftFront);
mesh.get().addVertex(topLeftFront);
mesh.get().addVertex(topLeftBack);

mesh.get().addTexCoord(0.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 1.0f);
mesh.get().addTexCoord(0.0f, 1.0f);

mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

elementOffset += elementOffsetIncrement;

mesh.get().addVertex(bottomRightBack);
mesh.get().addVertex(bottomLeftBack);
mesh.get().addVertex(topLeftBack);
mesh.get().addVertex(topRightBack);

mesh.get().addTexCoord(0.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 1.0f);
mesh.get().addTexCoord(0.0f, 1.0f);

mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

elementOffset += elementOffsetIncrement;

mesh.get().addVertex(bottomRightFront);
mesh.get().addVertex(bottomRightBack);
mesh.get().addVertex(topRightBack);
mesh.get().addVertex(topRightFront);

mesh.get().addTexCoord(0.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 1.0f);
mesh.get().addTexCoord(0.0f, 1.0f);

mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

elementOffset += elementOffsetIncrement;

mesh.get().addVertex(topLeftFront);
mesh.get().addVertex(topRightFront);
mesh.get().addVertex(topRightBack);
mesh.get().addVertex(topLeftBack);

mesh.get().addTexCoord(0.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 1.0f);
mesh.get().addTexCoord(0.0f, 1.0f);

mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

elementOffset += elementOffsetIncrement;

mesh.get().addVertex(bottomLeftBack);
mesh.get().addVertex(bottomRightBack);
mesh.get().addVertex(bottomRightFront);
mesh.get().addVertex(bottomLeftFront);

mesh.get().addTexCoord(0.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 0.0f);
mesh.get().addTexCoord(1.0f, 1.0f);
mesh.get().addTexCoord(0.0f, 1.0f);

mesh.get().addFace(0+elementOffset,1+elementOffset,2+elementOffset);
mesh.get().addFace(2+elementOffset,3+elementOffset,0+elementOffset);

elementOffset += elementOffsetIncrement;

return mesh;
}

