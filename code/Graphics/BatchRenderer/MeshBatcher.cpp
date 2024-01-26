#include "Graphics/BatchRenderer/MeshBatcher.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

void MeshBatcher::init(const BatchData batchData)
{
	PROFILER_CPU()
    mBatchData = batchData;

    mInternalMesh = OwnerPtr<Mesh>::newObject();

    initBuffers();
	
    enable();
    
	if(mBatchData.mIsInstanced)
	{
		initSingleMeshData();
	}

    disable();
}

void MeshBatcher::terminate()
{
    disable();
    mGPUBuffersLayout.terminate();
    // if(mGPUMeshBufferData.mIsSkinned)
    // {
    //     mBonesMatricesBuffer.terminate();
    // }
}

void MeshBatcher::addMeshDataToBuffers(Ptr<const Mesh> meshInstance)
{
    PROFILER_CPU()

    FOR_ARRAY(i, mInternalMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mInternalMesh->mGPUVertexInputBuffers[i];
        mInternalMesh->mBuffers.at(gpuVariableData.mName).append(meshInstance->mBuffers.at(gpuVariableData.mName));
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
    resizeMeshData(maxInstances);
}

void MeshBatcher::initSingleMeshData()
{
	PROFILER_CPU()
    initInternal(1);
    setMeshData(Ptr<const GPUMesh>::cast(mBatchData.mMesh));
    addMeshDataToBuffers(mBatchData.mMesh);
}

void MeshBatcher::resizeInternal(u32 maxInstances)
{
	PROFILER_CPU()

    mMatrices.reserve(maxInstances);

    generateInstanceIDsData(maxInstances);

    resizeInstancesData(maxInstances);
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
		GET_SYSTEM(GPUInterface).drawElements(GPUDrawPrimitive::TRIANGLES, mBatchData.mMesh->mIndices.size() * 3, mMeshesIndex, mBatchData.mIsInstanced);
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
            setBonesTransforms(transforms);
        }
	}
}

void MeshBatcher::enable()
{
    mGPUBuffersLayout.enable();
}

void MeshBatcher::disable()
{
    mGPUBuffersLayout.disable();
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

	setIndicesData(Ptr<const GPUMesh>::cast(mInternalMesh));
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
        setMeshData(Ptr<const GPUMesh>::cast(mInternalMesh));
    }
    setInstancesData(mMatrices, mInstanceIDs);
    mDataSentToGPU = true;
}

void MeshBatcher::initBuffers()
{
    PROFILER_CPU()

    mGPUBuffersLayout.init(mBatchData.mIsStatic || mBatchData.mIsInstanced);
    mGPUBuffersLayout.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mFace);

    FOR_ARRAY(i, mBatchData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mBatchData.mMesh->mGPUVertexInputBuffers[i];
        GPUVertexBufferData bufferData(gpuVariableData);
        mGPUBuffersLayout.createVertexBuffer(bufferData);
    }

    GPUVertexBufferData bufferDataInstanceIDs(GPUBuiltIn::VertexInput::mInstanceID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUBuffersLayout.createVertexBuffer(bufferDataInstanceIDs);
    
    mGPUBuffersLayout.disable();

    mGPUBuffersLayout.createInstanceBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices);

    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        u32 bonesMatricesBindingPoint = GET_SYSTEM(GPUSharedContext).requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mBonesMatrices.mType);
        mBonesMatricesBuffer.init(bonesMatricesBindingPoint, GPUBuiltIn::SharedBuffers::mBonesMatrices, mBatchData.mIsStatic);
        mBonesMatricesBuffer.resize<Matrix4>(GPUMesh::MAX_BONES);
    }
}

void MeshBatcher::resizeMeshData(u32 maxInstances)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mBatchData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mBatchData.mMesh->mGPUVertexInputBuffers[i];
        mGPUBuffersLayout.getVertexBuffer(gpuVariableData).resize(mBatchData.mMesh->mVertexCount * maxInstances);
    }
}

void MeshBatcher::resizeInstancesData(u32 maxInstances)
{
    PROFILER_CPU()
    u32 matricesBufferSizeMultiplier = mBatchData.mIsInstanced ? 1 : mBatchData.mMesh->mVertexCount;
    mGPUBuffersLayout.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).resize(maxInstances * matricesBufferSizeMultiplier);
    mGPUBuffersLayout.getInstanceBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).resize<Matrix4>(maxInstances);
}

void MeshBatcher::setMeshData(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mesh->mGPUVertexInputBuffers[i];
        mGPUBuffersLayout.getVertexBuffer(gpuVariableData).setDataArray(mesh->mBuffers.at(gpuVariableData.mName));
    }
}

void MeshBatcher::setInstancesData(const std::vector<Matrix4>& matrices, const std::vector<u32>& instanceIDs)
{
    PROFILER_CPU()
    PROFILER_BLOCK_CPU("VBO instanceIDs");
	mGPUBuffersLayout.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).setDataArray(instanceIDs);
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("UBO matrices");
    mGPUBuffersLayout.getInstanceBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).setDataArray(matrices);
    PROFILER_END_BLOCK();
}

void MeshBatcher::setBonesTransforms(const std::vector<Matrix4>& transforms)
{
    PROFILER_BLOCK_CPU("UBO Bones Transforms");
    mBonesMatricesBuffer.setDataArray(transforms);
    PROFILER_END_BLOCK();
}

void MeshBatcher::setIndicesData(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    mGPUBuffersLayout.getIndicesBuffer().resize(mesh->mIndices.size());
    mGPUBuffersLayout.getIndicesBuffer().setDataArray(mesh->mIndices);
}