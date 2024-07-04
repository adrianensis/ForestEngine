#pragma once

#include "Graphics/GPU/GPUMesh.hpp"

class MeshBatcher
{
public:
	MeshBatcher() = default;

    void init(Ptr<const GPUMesh> mesh);
    void resize(u32 size);
    void addInstanceData(u32 index, u32 objectId, u32 materialInstanceId);
private:
    void appendMeshData(Ptr<const GPUMesh> mesh);
    void allocateInstances(u32 maxInstances);
    void generateIndicesData(u32 meshesCount);
    void generateInstanceIDsData(u32 meshesCount);

private:
    Ptr<const GPUMesh> mMesh;
	OwnerPtr<GPUMesh> mInternalMesh;
    std::vector<u32> mInstanceIDs;
    std::vector<u32> mObjectIDs;
    std::vector<u32> mMaterialInstanceIDs;

public:
    GET(InternalMesh)
    CRGET(InstanceIDs)
    CRGET(ObjectIDs)
    CRGET(MaterialInstanceIDs)
};
