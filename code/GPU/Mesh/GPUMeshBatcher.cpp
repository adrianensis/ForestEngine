#include "GPU/Mesh/GPUMeshBatcher.hpp"

void GPUMeshBatcher::init(Core::WeakPtr<const GPUMesh> mesh, Core::u32 size)
{
	PROFILER_CPU()
    mMesh = mesh;
    mInternalMesh = Core::OwnerPtr<GPUMesh>::newObject();

    allocateInstances(1);
    appendMeshData(mMesh);
	resize(size);
}

void GPUMeshBatcher::appendMeshData(Core::WeakPtr<const GPUMesh> mesh)
{
    PROFILER_CPU()

    FOR_ARRAY(i, mInternalMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mInternalMesh->mGPUVertexInputBuffers[i];
        mInternalMesh->mBuffers.at(gpuVariableData.mName).append(mesh->mBuffers.at(gpuVariableData.mName));
    }
}

void GPUMeshBatcher::resize(Core::u32 size)
{
	PROFILER_CPU()

	mObjectIDs.resize(size);
	mGPUShaderPropertiesInstanceIDs.resize(size);
    mInstanceIDs.resize(size);
	generateInstanceIDsData(size);
}

void GPUMeshBatcher::allocateInstances(Core::u32 maxInstances)
{
	PROFILER_CPU()
    mInternalMesh->init(mMesh->mVertexCount * maxInstances, mMesh->mIndicesCount * maxInstances, mMesh->mGPUVertexInputBuffers);
    generateIndicesData(maxInstances);
}

void GPUMeshBatcher::setInstanceData(Core::u32 instanceId, Core::u32 objectId, Core::u32 shaderPropertiesInstanceId)
{
	PROFILER_CPU()

	CHECK_MSG(instanceId < mObjectIDs.size(), "Out of bounds!")
	CHECK_MSG(instanceId < mGPUShaderPropertiesInstanceIDs.size(), "Out of bounds!")
	
    mObjectIDs[instanceId] = (objectId);
    mGPUShaderPropertiesInstanceIDs[instanceId] = (shaderPropertiesInstanceId);
}

void GPUMeshBatcher::generateIndicesData(Core::u32 meshesCount)
{
	PROFILER_CPU()
	FOR_RANGE(i, 0, meshesCount)
	{
		Core::u32 offset = (i * mMesh->mVertexCount);
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

void GPUMeshBatcher::generateInstanceIDsData(Core::u32 meshesCount)
{
	PROFILER_CPU()
    FOR_RANGE(meshId, 0, meshesCount)
    {
        mInstanceIDs[meshId] = (meshId);
    }
}



