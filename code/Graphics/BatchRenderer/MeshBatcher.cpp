#include "Graphics/BatchRenderer/MeshBatcher.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Material/MaterialManager.hpp"
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
	
	if(mBatchData.mIsInstanced)
	{
        enable();
        initInternal(1);
        setMeshBuffers(Ptr<const GPUMesh>::cast(mBatchData.mMesh));
        addInstanceMeshData(mBatchData.mMesh);
        disable();
	}
}

void MeshBatcher::terminate()
{
    disable();
    mGPUVertexBuffersContainer.terminate();
}

void MeshBatcher::addInstanceMeshData(Ptr<const Mesh> meshInstance)
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
    resizeBuffers(maxInstances);
}

void MeshBatcher::resizeInternal(u32 maxInstances)
{
	PROFILER_CPU()

    generateInstanceIDsData(maxInstances);

    resizeInstancedBuffers(maxInstances);
    if (!mBatchData.mIsInstanced)
    {
        initInternal(maxInstances);
    }
}

void MeshBatcher::addInstanceData(const GPUInstanceSlot& gpuInstanceSlot, Ptr<const Mesh> meshInstance)
{
	PROFILER_CPU()

    if(mBatchData.mIsInstanced)
    {
        mObjectIDs.push_back(gpuInstanceSlot.getSlot());
    }
    else
	{
        FOR_RANGE(i, 0, mBatchData.mMesh->mVertexCount)
        {
            mObjectIDs.push_back(gpuInstanceSlot.getSlot());
        }
        
        addInstanceMeshData(meshInstance);
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
		bool isAnimated = mBatchData.mMesh->mModel->isAnimated() && GET_SYSTEM(MaterialManager).getMaterial(mBatchData.mMaterialId).getMaterialData().mIsSkinned;
        if(isAnimated)
        {
            const std::vector<Matrix4>& transforms = GET_SYSTEM(AnimationManager).getBoneTransforms(mBatchData.mMesh->mModel);
            setBonesTransformsBuffer(transforms);
        }
	}
}

void MeshBatcher::enable()
{
    mGPUVertexBuffersContainer.enable();
}

void MeshBatcher::disable()
{
    mGPUVertexBuffersContainer.disable();
}

void MeshBatcher::clear()
{
	PROFILER_CPU()
	if( ! mBatchData.mIsInstanced)
	{
		mInternalMesh->clear();
	}
    mObjectIDs.clear();
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

	setIndicesBuffer(Ptr<const GPUMesh>::cast(mInternalMesh));
}

void MeshBatcher::generateInstanceIDsData(u32 meshesCount)
{
	PROFILER_CPU()

    u32 totalSize = meshesCount * (mBatchData.mIsInstanced ? 1 : mBatchData.mMesh->mVertexCount);

	mObjectIDs.clear();
	mObjectIDs.reserve(totalSize);

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

void MeshBatcher::sendDataToGPU()
{	
    PROFILER_CPU()
    if(!mBatchData.mIsInstanced)
	{
        setMeshBuffers(Ptr<const GPUMesh>::cast(mInternalMesh));
    }
    setInstancedBuffers();
    mDataSentToGPU = true;
}

void MeshBatcher::initBuffers()
{
    PROFILER_CPU()

    bool isStatic = mBatchData.mIsStatic || mBatchData.mIsInstanced;
    mGPUVertexBuffersContainer.init();
    mGPUVertexBuffersContainer.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mFace, isStatic);

    FOR_ARRAY(i, mBatchData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mBatchData.mMesh->mGPUVertexInputBuffers[i];
        GPUVertexBufferData bufferData(gpuVariableData);
        mGPUVertexBuffersContainer.createVertexBuffer(bufferData, isStatic);
    }

    GPUVertexBufferData bufferDataInstanceIDs(GPUBuiltIn::VertexInput::mInstanceID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUVertexBuffersContainer.createVertexBuffer(bufferDataInstanceIDs, isStatic);

    GPUVertexBufferData bufferDataMarixIDs(GPUBuiltIn::VertexInput::mObjectID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUVertexBuffersContainer.createVertexBuffer(bufferDataMarixIDs, isStatic);
    
    mGPUVertexBuffersContainer.disable();

    if(GET_SYSTEM(MaterialManager).getMaterial(mBatchData.mMaterialId).getMaterialData().mIsSkinned)
    {
        u32 bindingPointBoneMatrices = GET_SYSTEM(GPUSharedContext).requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mBonesMatrices.mType);
        mGPUSharedBuffersContainer.createSharedBuffer(bindingPointBoneMatrices, GPUBuiltIn::SharedBuffers::mBonesMatrices, isStatic);
        mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).resize<Matrix4>(GPUBuiltIn::MAX_BONES);
    }
}

void MeshBatcher::resizeBuffers(u32 maxInstances)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mBatchData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mBatchData.mMesh->mGPUVertexInputBuffers[i];
        mGPUVertexBuffersContainer.getVertexBuffer(gpuVariableData).resize(mBatchData.mMesh->mVertexCount * maxInstances);
    }
}

void MeshBatcher::resizeInstancedBuffers(u32 maxInstances)
{
    PROFILER_CPU()
    u32 matricesBufferSizeMultiplier = mBatchData.mIsInstanced ? 1 : mBatchData.mMesh->mVertexCount;
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).resize(maxInstances * matricesBufferSizeMultiplier);
    if(GET_SYSTEM(MaterialManager).getMaterial(mBatchData.mMaterialId).getMaterialData().mUseModelMatrix)
    {
	    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mObjectID).resize(maxInstances * matricesBufferSizeMultiplier);
    }
}

void MeshBatcher::setMeshBuffers(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mesh->mGPUVertexInputBuffers[i];
        mGPUVertexBuffersContainer.getVertexBuffer(gpuVariableData).setDataArray(mesh->mBuffers.at(gpuVariableData.mName));
    }
}

void MeshBatcher::setInstancedBuffers()
{
    PROFILER_CPU()
	mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).setDataArray(mInstanceIDs);
    if(GET_SYSTEM(MaterialManager).getMaterial(mBatchData.mMaterialId).getMaterialData().mUseModelMatrix)
    {
	    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mObjectID).setDataArray(mObjectIDs);
    }
}

void MeshBatcher::setBonesTransformsBuffer(const std::vector<Matrix4>& transforms)
{
    PROFILER_CPU()
    mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).setDataArray(transforms);
}

void MeshBatcher::setIndicesBuffer(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    mGPUVertexBuffersContainer.getIndicesBuffer().resize(mesh->mIndices.size());
    mGPUVertexBuffersContainer.getIndicesBuffer().setDataArray(mesh->mIndices);
}