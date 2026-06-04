#pragma once

#include "GPU/Mesh/GPUMesh.hpp"

class GPUMeshBatcher
{
public:
	GPUMeshBatcher() = default;

    void init(Core::WeakPtr<const GPUMesh> mesh, GPU::u32 size);
    void resize(GPU::u32 size);
    void setInstanceData(GPU::u32 instanceId, GPU::u32 objectId, GPU::u32 shaderPropertiesInstanceId);
private:
    void appendMeshData(Core::WeakPtr<const GPUMesh> mesh);
    void allocateInstances(GPU::u32 maxInstances);
    void generateIndicesData(GPU::u32 meshesCount);
    void generateInstanceIDsData(GPU::u32 meshesCount);

private:
    Core::WeakPtr<const GPUMesh> mMesh;
	Core::OwnerPtr<GPUMesh> mInternalMesh;
    std::vector<GPU::u32> mInstanceIDs;
    std::vector<GPU::u32> mObjectIDs;
    std::vector<GPU::u32> mGPUShaderPropertiesInstanceIDs;

public:
    GET(InternalMesh)
    CRGET(InstanceIDs)
    CRGET(ObjectIDs)
    CRGET(GPUShaderPropertiesInstanceIDs)
};
