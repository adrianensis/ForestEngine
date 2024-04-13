#pragma once

#include "Graphics/GPU/GPUMesh.hpp"

class Model;
class Material;

class BoneVertexIDsData
{
public:
    i32 mBonesIDs[GPUBuiltIn::MAX_BONE_INFLUENCE] = {-1, -1, -1, -1};
};

class BoneVertexWeightsData
{
public:
    f32 mBonesWeights[GPUBuiltIn::MAX_BONE_INFLUENCE] = {0.0f, 0.0f, 0.0f, 0.0f};
};

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
