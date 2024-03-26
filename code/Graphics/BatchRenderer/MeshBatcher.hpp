#pragma once

#include "Graphics/BatchRenderer/BatchData.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

class MeshBatcher
{
public:
	MeshBatcher() = default;

    void init(const BatchData batchData);
    void resize(u32 size);
    void addInstanceData(Ptr<MeshRenderer> renderer);
    void allocateInstances(u32 maxInstances);
    void appendMeshData(Ptr<const Mesh> mesh);
    void clear();

private:
    void generateIndicesData(u32 meshesCount);
    void generateInstanceIDsData(u32 meshesCount);

private:
    BatchData mBatchData;
	OwnerPtr<Mesh> mInternalMesh;
    std::vector<u32> mInstanceIDs;
    std::vector<u32> mObjectIDs;
    std::vector<u32> mMaterialInstanceIDs;

public:
    GET(InternalMesh)
    CRGET(InstanceIDs)
    CRGET(ObjectIDs)
    CRGET(MaterialInstanceIDs)
};
