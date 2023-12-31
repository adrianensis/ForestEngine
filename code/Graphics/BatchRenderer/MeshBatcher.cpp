#include "Graphics/BatchRenderer/MeshBatcher.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

void MeshBatcher::init(const BatchData batchData)
{
	PROFILER_CPU()
    mBatchData = batchData;

    mInternalMesh = OwnerPtr<Mesh>::newObject();

    GPUMeshBufferData gpuMeshBufferData;
	gpuMeshBufferData.mIsStatic = mBatchData.mIsStatic;
	gpuMeshBufferData.mIsInstanced = mBatchData.mIsInstanced;
	gpuMeshBufferData.mIsSkinned = mBatchData.mMaterial->getMaterialData().mIsSkinned;
	gpuMeshBufferData.mMesh = Ptr<const GPUMesh>::cast(mBatchData.mMesh);

	mGPUMeshBuffer.init(gpuMeshBufferData);
	
    enable();
    
	if(mBatchData.mIsInstanced)
	{
		initSingleMeshData();
	}

    disable();
}

void MeshBatcher::terminate()
{
    mGPUMeshBuffer.terminate();
}

void MeshBatcher::addMeshDataToBuffers(Ptr<const Mesh> meshInstance)
{
    PROFILER_CPU()

    FOR_ARRAY(i, mInternalMesh->mGPUVertexInputBuffers.mBuffers)
    {
        const GPUVariableData& gpuVariableData = mInternalMesh->mGPUVertexInputBuffers.mBuffers[i];
        mInternalMesh->mGPUMeshByteBuffers.mBuffers.at(gpuVariableData.mName).append(meshInstance->mGPUMeshByteBuffers.mBuffers.at(gpuVariableData.mName));
    }
}

void MeshBatcher::resize(u32 size)
{
	PROFILER_CPU()

	clear();

	if (size > mMaxMeshesThreshold)
	{
        PROFILER_BLOCK_CPU("new size");
		if(mMaxMeshesThreshold == 0)
		{
			mMaxMeshesThreshold = size;
		}
		else
		{
			mMaxMeshesThreshold += mMaxMeshesIncrement;
		}

        resizeInternal(mMaxMeshesThreshold);
        PROFILER_END_BLOCK();
	}

	mMeshesIndex = 0;

	mDataSentToGPU = false;
}

void MeshBatcher::initInternal(u32 maxInstances)
{
	PROFILER_CPU()
    mInternalMesh->init(mBatchData.mMesh->mVertexCount * maxInstances, mBatchData.mMesh->mIndicesCount * maxInstances, mBatchData.mMesh->mGPUVertexInputBuffers);
    generateIndicesData(maxInstances);
    mGPUMeshBuffer.resizeMeshData(maxInstances);
}

void MeshBatcher::initSingleMeshData()
{
	PROFILER_CPU()
    initInternal(1);
    mGPUMeshBuffer.setMeshData(Ptr<const GPUMesh>::cast(mBatchData.mMesh));
    addMeshDataToBuffers(mBatchData.mMesh);
}

void MeshBatcher::resizeInternal(u32 maxInstances)
{
	PROFILER_CPU()

    mMatrices.reserve(maxInstances);

    generateInstanceIDsData(maxInstances);

    mGPUMeshBuffer.resizeInstancesData(maxInstances);
    if (!mBatchData.mIsInstanced)
    {
        initInternal(maxInstances);
    }
}

void MeshBatcher::addInstance(const Matrix4& modelMatrix, Ptr<const Mesh> meshInstance)
{
	PROFILER_CPU()

    mMatrices.push_back(modelMatrix);

    if(!mBatchData.mIsInstanced)
	{
        PROFILER_BLOCK_CPU("Non Instanced");
        addMeshDataToBuffers(meshInstance);
    }

	mMeshesIndex++;
}

void MeshBatcher::drawCall()
{	
	if (mMeshesIndex > 0)
	{
        if(!mDataSentToGPU)
        {
		    sendDataToGPU();
        }
		GET_SYSTEM(GPUInterface).drawElements(GL_TRIANGLES, mBatchData.mMesh->mIndices.size() * 3, mMeshesIndex, mBatchData.mIsInstanced);
	}
}

void MeshBatcher::updateBoneTransforms()
{	
	if(mBatchData.mMesh->mModel)
	{
		bool isAnimated = mBatchData.mMesh->mModel->isAnimated() && mBatchData.mMaterial->getMaterialData().mIsSkinned;
        if(isAnimated)
        {
            const std::vector<Matrix4>& transforms = GET_SYSTEM(AnimationManager).getBoneTransforms(mBatchData.mMesh->mModel);
            mGPUMeshBuffer.setBonesTransforms(transforms);
        }
	}
}

void MeshBatcher::enable()
{
	mGPUMeshBuffer.enable();
}

void MeshBatcher::disable()
{
	mGPUMeshBuffer.disable();
}

void MeshBatcher::clear()
{
	PROFILER_CPU()
	if( ! mBatchData.mIsInstanced)
	{
		mInternalMesh->clear();
	}
    mMatrices.clear();
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

	mGPUMeshBuffer.setIndicesData(Ptr<const GPUMesh>::cast(mInternalMesh));
}

void MeshBatcher::generateInstanceIDsData(u32 meshesCount)
{
	PROFILER_CPU()

	mInstanceIDs.clear();
    mInstanceIDs.reserve(meshesCount * (mBatchData.mIsInstanced ? 1 : mBatchData.mMesh->mVertexCount));
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

void MeshBatcher::sendDataToGPU()
{	
    PROFILER_CPU()
    if(!mBatchData.mIsInstanced)
	{
        mGPUMeshBuffer.setMeshData(Ptr<const GPUMesh>::cast(mInternalMesh));
    }
    mGPUMeshBuffer.setInstancesData(mMatrices, mInstanceIDs);
    mDataSentToGPU = true;
}
