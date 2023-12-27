#include "Graphics/BatchRenderer/MeshBatcher.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"

void MeshBatcher::init(const BatchData batchData)
{
	PROFILER_CPU()
    mBatchData = batchData;

    mInternalMesh = OwnerPtr<Mesh>::newObject();

    GPUMeshBufferData gpuMeshBufferData;
	gpuMeshBufferData.mVertexCount = mBatchData.mMesh->mVertexCount;
	gpuMeshBufferData.mIsStatic = mBatchData.mIsStatic;
	gpuMeshBufferData.mIsInstanced = mBatchData.mIsInstanced;
	gpuMeshBufferData.mUseVertexColor = mBatchData.mMaterial->getMaterialData().mUseVertexColor;

	mGPUMeshBuffer.init(gpuMeshBufferData);
	
    enable();
    
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
    mInternalMesh->mPositions.append(meshInstance->mPositions);
    PROFILER_END_BLOCK();

    PROFILER_BLOCK_CPU("TextureCoordinates");
    mInternalMesh->mTextureCoordinates.append(meshInstance->mTextureCoordinates);
    PROFILER_END_BLOCK();

    if(mBatchData.mMaterial->getMaterialData().mUseVertexColor)
    {
        PROFILER_BLOCK_CPU("Colors");
        mInternalMesh->mColors.append(meshInstance->mColors);
    }

    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        PROFILER_BLOCK_CPU("BonesVertexIDsData");
        mInternalMesh->mBonesVertexIDsData.append(meshInstance->mBonesVertexIDsData);
        PROFILER_END_BLOCK();

        PROFILER_BLOCK_CPU("BonesVertexWeightsData");
        mInternalMesh->mBonesVertexWeightsData.append(meshInstance->mBonesVertexWeightsData);
        PROFILER_END_BLOCK();
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
    mInternalMesh->init(mBatchData.mMesh->mVertexCount * maxInstances, mBatchData.mMesh->mFacesCount * maxInstances);
    generateFacesData(maxInstances);
    mGPUMeshBuffer.resizeMeshData(Ptr<const GPUMesh>::cast(mInternalMesh));
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
		GET_SYSTEM(GPUInterface).drawElements(GL_TRIANGLES, mBatchData.mMesh->mFaces.size<Face>() * 3, mMeshesIndex, mBatchData.mIsInstanced);
	}
}

void MeshBatcher::updateBoneTransforms()
{	
	if(mBatchData.mMesh->mModel)
	{
		bool isAnimated = mBatchData.mMesh->mModel->isAnimated();
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

void MeshBatcher::generateFacesData(u32 meshesCount)
{
	PROFILER_CPU()

	FOR_RANGE(i, 0, meshesCount)
	{
		u32 offset = (i * mBatchData.mMesh->mVertexCount);
		
		FOR_RANGE(faceIndex, 0, mBatchData.mMesh->mFaces.size<Face>())
		{
			Face newFace = mBatchData.mMesh->mFaces.get<Face>(faceIndex);
			newFace.mIndex0 += offset;
			newFace.mIndex1 += offset;
			newFace.mIndex2 += offset;

			mInternalMesh->mFaces.pushBack(newFace);
		}
	}

	mGPUMeshBuffer.setIndicesData(Ptr<const GPUMesh>::cast(mInternalMesh));
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
