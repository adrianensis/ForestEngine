#include "Graphics/GPU/Mesh/GPUMeshBatcher.hpp"

void GPUMeshBatcher::init(Ptr<const GPUMesh> mesh)
{
	PROFILER_CPU()
    mMesh = mesh;
    mInternalMesh = OwnerPtr<GPUMesh>::newObject();

    allocateInstances(1);
    appendMeshData(mMesh);
}

void GPUMeshBatcher::appendMeshData(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()

    FOR_ARRAY(i, mInternalMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mInternalMesh->mGPUVertexInputBuffers[i];
        mInternalMesh->mBuffers.at(gpuVariableData.mName).append(mesh->mBuffers.at(gpuVariableData.mName));
    }
}

void GPUMeshBatcher::resize(u32 size)
{
	PROFILER_CPU()

	mObjectIDs.resize(size);
	mMaterialInstanceIDs.resize(size);
    mInstanceIDs.resize(size);
	generateInstanceIDsData(size);
}

void GPUMeshBatcher::allocateInstances(u32 maxInstances)
{
	PROFILER_CPU()
    mInternalMesh->init(mMesh->mVertexCount * maxInstances, mMesh->mIndicesCount * maxInstances, mMesh->mGPUVertexInputBuffers);
    generateIndicesData(maxInstances);
}

void GPUMeshBatcher::setInstanceData(u32 instanceId, u32 objectId, u32 materialInstanceId)
{
	PROFILER_CPU()

    mObjectIDs[instanceId] = (objectId);
    mMaterialInstanceIDs[instanceId] = (materialInstanceId);
}

void GPUMeshBatcher::generateIndicesData(u32 meshesCount)
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

void GPUMeshBatcher::generateInstanceIDsData(u32 meshesCount)
{
	PROFILER_CPU()
    FOR_RANGE(meshId, 0, meshesCount)
    {
        mInstanceIDs[meshId] = (meshId);
    }
}



