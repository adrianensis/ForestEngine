#include "Graphics/BatchRenderer/MeshBatcher.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/RenderSharedContext.hpp"

void MeshBatcher::init(const BatchData batchData)
{
	PROFILER_CPU()
    mBatchData = batchData;
    mInternalMesh = OwnerPtr<Mesh>::newObject();
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

	generateInstanceIDsData(size);

    if (!mBatchData.mIsInstanced)
    {
        allocateInstances(size);
    }
}

void MeshBatcher::allocateInstances(u32 maxInstances)
{
	PROFILER_CPU()
    mInternalMesh->init(mBatchData.mMesh->mVertexCount * maxInstances, mBatchData.mMesh->mIndicesCount * maxInstances, mBatchData.mMesh->mGPUVertexInputBuffers);
    generateIndicesData(maxInstances);
}

void MeshBatcher::addInstanceData(Ptr<MeshRenderer> renderer)
{
	PROFILER_CPU()

    if(mBatchData.mIsInstanced)
    {
        mObjectIDs.push_back(renderer->getRenderInstanceSlot().getSlot());
        mMaterialInstanceIDs.push_back(renderer->getMaterialInstanceSlot().getSlot());
    }
    else
	{
        FOR_RANGE(i, 0, mBatchData.mMesh->mVertexCount)
        {
            mObjectIDs.push_back(renderer->getRenderInstanceSlot().getSlot());
            mMaterialInstanceIDs.push_back(renderer->getMaterialInstanceSlot().getSlot());
        }
        
        appendMeshData(renderer->getMeshInstance());
    }
}

void MeshBatcher::clear()
{
	PROFILER_CPU()
	if( ! mBatchData.mIsInstanced)
	{
		mInternalMesh->clear();
	}
    mObjectIDs.clear();
    mMaterialInstanceIDs.clear();
}

void MeshBatcher::generateIndicesData(u32 meshesCount)
{
	PROFILER_CPU()

	FOR_RANGE(i, 0, meshesCount)
	{
		u32 offset = (i * mBatchData.mMesh->mVertexCount);
		
		FOR_RANGE(faceIndex, 0, mBatchData.mMesh->mIndices.size())
		{
			Face newFace = mBatchData.mMesh->mIndices.get<Face>(faceIndex);
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

    u32 totalSize = meshesCount * (mBatchData.mIsInstanced ? 1 : mBatchData.mMesh->mVertexCount);

	mObjectIDs.clear();
	mObjectIDs.reserve(totalSize);
	mMaterialInstanceIDs.clear();
	mMaterialInstanceIDs.reserve(totalSize);
	mInstanceIDs.clear();
    mInstanceIDs.reserve(totalSize);

    FOR_RANGE(meshId, 0, meshesCount)
    {
        if(mBatchData.mIsInstanced)
        {
            mInstanceIDs.push_back(meshId);
        }
        else
        {
            FOR_RANGE(i, 0, mBatchData.mMesh->mVertexCount)
            {
                mInstanceIDs.push_back(meshId);
            }
        }
    }
}



