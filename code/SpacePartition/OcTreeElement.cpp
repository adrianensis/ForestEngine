#include "SpacePartition/OcTreeElement.hpp"

void IOcTreeElement::init(const Matrix4& modelMatrix, const Vector3& meshMin, const Vector3& meshMax)
{
    Vector3 maxWorld = modelMatrix.mulVector(Vector4(meshMax, 1));
    Vector3 minWorld = modelMatrix.mulVector(Vector4(meshMin, 1));
    Vector3 diffWorld(maxWorld - minWorld);
    diffWorld.abs();
    Vector3 centerWorld = maxWorld - diffWorld/2.0f;
    Vector3 topLeft = centerWorld + (Vector3(-diffWorld.x,diffWorld.y,diffWorld.z)/2.0f);

    mOcTreeBoundingBox = Cube(topLeft, diffWorld);
}
