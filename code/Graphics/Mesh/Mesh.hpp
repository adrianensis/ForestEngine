#pragma once

#include "Graphics/GPU/GPUMesh.hpp"

class Model;
class Material;

class Mesh: public GPUMesh
{
public:
    void addBoneWeight(u32 vertexId, i32 id, f32 weight);
    std::vector<Vector3> calculateSkinnedVertices() const;

private:
    Vector3 calculateSkinnedVertex(u32 i) const;

public:
    Ptr<const Model> mModel;
};
REGISTER_CLASS(Mesh);
