#include "Graphics/Batch/MeshBatcher.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

void MeshBatcher::init(const BatchData batchData)
{
	PROFILER_CPU()
    mBatchData = batchData;

    GPUMeshBufferData gpuMeshBufferData;
	gpuMeshBufferData.mVertexCount = mBatchData.mMesh->mVertexCount;
	gpuMeshBufferData.mIsStatic = mBatchData.mIsStatic;
	gpuMeshBufferData.mIsInstanced = mBatchData.mIsInstanced;
	gpuMeshBufferData.mUseVertexColor = mBatchData.mMaterial->getMaterialData().mUseVertexColor;

	mGPUMeshBuffer.init(gpuMeshBufferData);
	
	if(mBatchData.mIsInstanced)
	{
		initSingleMeshData();
	}

    disable();
}

void MeshBatcher::addMeshDataToBuffers(Ptr<const Mesh> meshInstance)
{
    PROFILER_CPU()

    PROFILER_BLOCK_CPU("Positions");
    mMeshBuilder.appendToPositions(meshInstance->mPositions);
    PROFILER_END_BLOCK();

    PROFILER_BLOCK_CPU("TextureCoordinates");
    mMeshBuilder.appendToTextureCoordinates(meshInstance->mTextureCoordinates);
    PROFILER_END_BLOCK();

    PROFILER_BLOCK_CPU("Colors");
    if(mBatchData.mMaterial->getMaterialData().mUseVertexColor)
    {
        mMeshBuilder.appendToColors(meshInstance->mColors);
    }
    PROFILER_END_BLOCK();

    PROFILER_BLOCK_CPU("BonesVertexIDsData");
    mMeshBuilder.appendToBonesVertexIDsData(meshInstance->mBonesVertexIDsData);
    PROFILER_END_BLOCK();

    PROFILER_BLOCK_CPU("BonesVertexWeightsData");
    mMeshBuilder.appendToBonesVertexWeightsData(meshInstance->mBonesVertexWeightsData);
    PROFILER_END_BLOCK();
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
    mMeshBuilder.init(mBatchData.mMesh->mVertexCount * maxInstances, mBatchData.mMesh->mFacesCount * maxInstances);
    generateFacesData(maxInstances);
    mGPUMeshBuffer.resizeMeshData(mMeshBuilder);
}

void MeshBatcher::initSingleMeshData()
{
	PROFILER_CPU()
    initInternal(1);
    mGPUMeshBuffer.setMeshData(mBatchData.mMesh.get());
    addMeshDataToBuffers(mBatchData.mMesh);
}

void MeshBatcher::resizeInternal(u32 maxInstances)
{
	PROFILER_CPU()

    mMatrices.reserve(maxInstances);

    PROFILER_BLOCK_CPU("instances IDs");
    mInstanceIDs.clear();
    mInstanceIDs.reserve(maxInstances * (mBatchData.mIsInstanced ? 1 : mBatchData.mMesh->mVertexCount));
    FOR_RANGE(meshId, 0, maxInstances)
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
    PROFILER_END_BLOCK();

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

    if(mBatchData.mIsInstanced)
	{
        PROFILER_BLOCK_CPU("Instanced");
    }
    else
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
		GET_SYSTEM(GPUInterface).drawElements(mBatchData.mMesh->mFaces.size() * 3, mMeshesIndex, mBatchData.mIsInstanced);
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
		mMeshBuilder.clear();
	}
    mMatrices.clear();
}

void MeshBatcher::generateFacesData(u32 meshesCount)
{
	PROFILER_CPU()

	FOR_RANGE(i, 0, meshesCount)
	{
		u32 offset = (i * mBatchData.mMesh->mVertexCount);
		
		FOR_RANGE(faceIndex, 0, mBatchData.mMesh->mFaces.size())
		{
			Face newFace = mBatchData.mMesh->mFaces[faceIndex];
			newFace.mIndex0 += offset;
			newFace.mIndex1 += offset;
			newFace.mIndex2 += offset;

			mMeshBuilder.addToFaces(newFace);
		}
	}

	mGPUMeshBuffer.setIndexesData(mMeshBuilder);
}

void MeshBatcher::sendDataToGPU()
{	
    PROFILER_CPU()
    if(!mBatchData.mIsInstanced)
	{
        mGPUMeshBuffer.setMeshData(mMeshBuilder);
    }
    mGPUMeshBuffer.setInstancesData(mMatrices, mInstanceIDs);
    mDataSentToGPU = true;
}
