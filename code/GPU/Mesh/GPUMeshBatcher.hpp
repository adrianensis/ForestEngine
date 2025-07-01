#pragma once

#include "GPU/Mesh/GPUMesh.hpp"

class GPUMeshBatcher
{
public:
	GPUMeshBatcher() = default;

    void init(Core::WeakPtr<const GPUMesh> mesh, Core::u32 size);
    void resize(Core::u32 size);
    void setInstanceData(Core::u32 instanceId, Core::u32 objectId, Core::u32 shaderPropertiesInstanceId);
private:
    void appendMeshData(Core::WeakPtr<const GPUMesh> mesh);
    void allocateInstances(Core::u32 maxInstances);
    void generateIndicesData(Core::u32 meshesCount);
    void generateInstanceIDsData(Core::u32 meshesCount);

private:
    Core::WeakPtr<const GPUMesh> mMesh;
	Core::OwnerPtr<GPUMesh> mInternalMesh;
    std::vector<Core::u32> mInstanceIDs;
    std::vector<Core::u32> mObjectIDs;
    std::vector<Core::u32> mGPUShaderPropertiesInstanceIDs;

public:
    GET(InternalMesh)
    CRGET(InstanceIDs)
    CRGET(ObjectIDs)
    CRGET(GPUShaderPropertiesInstanceIDs)
};
