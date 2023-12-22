#pragma once

#include "Graphics/GPU/GPUMesh.hpp"

class Model;
class Material;

class Mesh: public GPUMesh
{
    GENERATE_METADATA(Mesh)

public:
    std::vector<Vector3> calculateSkinnedVertices() const;

private:
    Vector3 calculateSkinnedVertex(u32 i) const;

public:
    Ptr<const Model> mModel;
};
