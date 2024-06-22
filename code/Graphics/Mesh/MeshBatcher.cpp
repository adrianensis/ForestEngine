#include "Graphics/Mesh/MeshBatcher.hpp"

void MeshBatcher::init(Ptr<const Mesh> mesh)
{
	PROFILER_CPU()
    mMesh = mesh;
    mInternalMesh = OwnerPtr<Mesh>::newObject();

    allocateInstances(1);
    appendMeshData(mMesh);
}

void MeshBatcher::appendMeshData(Ptr<const Mesh> mesh)
{
    PROFILER_CPU()

    FOR_ARRAY(i, mInternalMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mInternalMesh->mGPUVertexInputBuffers[i];
        mInternalMesh->mBuffers.at(gpuVariableData.mName).append(mesh->mBuffers.at(gpuVariableData.mName));
    }
}

void MeshBatcher::resize(u32 size)
{
	PROFILER_CPU()

	mObjectIDs.resize(size);
	mMaterialInstanceIDs.resize(size);
    mInstanceIDs.resize(size);
	generateInstanceIDsData(size);
}

void MeshBatcher::allocateInstances(u32 maxInstances)
{
	PROFILER_CPU()
    mInternalMesh->init(mMesh->mVertexCount * maxInstances, mMesh->mIndicesCount * maxInstances, mMesh->mGPUVertexInputBuffers);
    generateIndicesData(maxInstances);
}

void MeshBatcher::addInstanceData(u32 index, Ptr<const Mesh> meshInstance, u32 objectId, u32 materialInstanceId)
{
	PROFILER_CPU()

    mObjectIDs[index] = (objectId);
    mMaterialInstanceIDs[index] = (materialInstanceId);
}

void MeshBatcher::generateIndicesData(u32 meshesCount)
{
	PROFILER_CPU()
	FOR_RANGE(i, 0, meshesCount)
	{
		u32 offset = (i * mMesh->mVertexCount);
		FOR_RANGE(faceIndex, 0, mMesh->mIndices.size())
		{
			Face newFace = mMesh->mIndices.get<Face>(faceIndex);
			newFace.mIndex0 += offset;
			newFace.mIndex1 += offset;
			newFace.mIndex2 += offset;
			mInternalMesh->mIndices.pushBack(newFace);
		}
	}
}

void MeshBatcher::generateInstanceIDsData(u32 meshesCount)
{
	PROFILER_CPU()
    FOR_RANGE(meshId, 0, meshesCount)
    {
        mInstanceIDs[meshId] = (meshId);
    }
}



