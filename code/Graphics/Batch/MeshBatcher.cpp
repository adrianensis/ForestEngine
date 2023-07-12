#include "Graphics/Batch/MeshBatcher.hpp"
#include "Graphics/Model/Model.hpp"

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
		mMeshBuilder.init(mBatchData.mMesh->mVertexCount, mBatchData.mMesh->mFacesCount);

        addDataToBuffers(mBatchData.mMesh);

		generateFacesData(1);
		mGPUMeshBuffer.resizeMeshData(mBatchData.mMesh.get());

		mDataSentToGPU = false;

		mGPUMeshBuffer.setMeshData(mBatchData.mMesh.get());

		mMeshesIndex = 0;
	}

    disable();
}

void MeshBatcher::addDataToBuffers(Ptr<const Mesh> meshInstance)
{
    PROFILER_CPU()
    mMeshBuilder.appendToPositions(meshInstance->mPositions);
    mMeshBuilder.appendToTextureCoordinates(meshInstance->mTextureCoordinates);
    if(mBatchData.mMaterial->getMaterialData().mUseVertexColor)
    {
        mMeshBuilder.appendToColors(meshInstance->mColors);
    }
    mMeshBuilder.appendToBonesVertexIDsData(meshInstance->mBonesVertexIDsData);
    mMeshBuilder.appendToBonesVertexWeightsData(meshInstance->mBonesVertexWeightsData);
}

void MeshBatcher::resize(u32 size)
{
	PROFILER_CPU()

	clear();

	if (size > mMaxMeshesThreshold)
	{
		if(mMaxMeshesThreshold == 0)
		{
			mMaxMeshesThreshold = size;
		}
		else
		{
			mMaxMeshesThreshold += mMaxMeshesIncrement;
		}

        mGPUMeshBuffer.resizeInstancesData(mMaxMeshesThreshold);
		if (!mBatchData.mIsInstanced)
		{
			mMeshBuilder.init(mBatchData.mMesh->mVertexCount * mMaxMeshesThreshold, mBatchData.mMesh->mFacesCount * mMaxMeshesThreshold);
			generateFacesData(mMaxMeshesThreshold);
			mGPUMeshBuffer.resizeMeshData(mMeshBuilder);
		}
	}

	mMeshesIndex = 0;

	mDataSentToGPU = false;
}

void MeshBatcher::addInstance(const Matrix4& modelMatrix, Ptr<const Mesh> meshInstance)
{
	PROFILER_CPU()

    if(mBatchData.mIsInstanced)
	{
	    mMatrices.push_back(modelMatrix);
    }
    else
    {
        FOR_RANGE(i, 0, mBatchData.mMesh->mVertexCount)
        {
            mMatrices.push_back(modelMatrix);
        }

        addDataToBuffers(meshInstance);
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
		GET_SYSTEM(RenderContext).drawElements(mBatchData.mMesh->mFaces.size() * 3, mMeshesIndex, mBatchData.mIsInstanced);
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
    mGPUMeshBuffer.setInstancesData(mMatrices);
    mDataSentToGPU = true;
}
