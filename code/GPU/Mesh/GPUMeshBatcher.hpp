#pragma once

#include "GPU/Mesh/GPUMesh.hpp"

class GPUMeshBatcher
{
public:
	GPUMeshBatcher() = default;

    void init(WeakPtr<const GPUMesh> mesh, u32 size);
    void resize(u32 size);
    void setInstanceData(u32 instanceId, u32 objectId, u32 shaderInstanceId);
private:
    void appendMeshData(WeakPtr<const GPUMesh> mesh);
    void allocateInstances(u32 maxInstances);
    void generateIndicesData(u32 meshesCount);
    void generateInstanceIDsData(u32 meshesCount);

private:
    WeakPtr<const GPUMesh> mMesh;
	OwnerPtr<GPUMesh> mInternalMesh;
    std::vector<u32> mInstanceIDs;
    std::vector<u32> mObjectIDs;
    std::vector<u32> mShaderInstanceIDs;

public:
    GET(InternalMesh)
    CRGET(InstanceIDs)
    CRGET(ObjectIDs)
    CRGET(ShaderInstanceIDs)
};
