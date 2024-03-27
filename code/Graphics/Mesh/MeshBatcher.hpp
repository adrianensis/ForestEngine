#pragma once

#include "Graphics/Mesh/Mesh.hpp"

class MeshBatcher
{
public:
	MeshBatcher() = default;

    void init(Ptr<const Mesh> mesh, bool isInstanced);
    void resize(u32 size);
    void addInstanceData(Ptr<const Mesh> meshInstance, u32 objectId, u32 materialInstanceId);
    void clear();

private:
    void appendMeshData(Ptr<const Mesh> mesh);
    void allocateInstances(u32 maxInstances);
    void generateIndicesData(u32 meshesCount);
    void generateInstanceIDsData(u32 meshesCount);

private:
    Ptr<const Mesh> mMesh;
    bool mIsInstanced = false;
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
