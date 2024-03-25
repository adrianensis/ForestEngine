#include "Graphics/BatchRenderer/MeshBatcher.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/RenderSharedContext.hpp"
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
        appendMeshData(mBatchData.mMesh);
        disable();
	}
}

void MeshBatcher::terminate()
{
    disable();
    mGPUVertexBuffersContainer.terminate();
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

	mDataSubmittedToGPU = false;
}

void MeshBatcher::initInternal(u32 maxInstances)
{
	PROFILER_CPU()
    mInternalMesh->init(mBatchData.mMesh->mVertexCount * maxInstances, mBatchData.mMesh->mIndicesCount * maxInstances, mBatchData.mMesh->mGPUVertexInputBuffers);
    generateIndicesData(maxInstances);
    resizeMeshBuffers(maxInstances);
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

void MeshBatcher::updateBoneTransforms()
{	
	if(mBatchData.mMesh->mModel)
	{
		bool isAnimated = mBatchData.mMesh->mModel->isAnimated() && mBatchData.mMaterial->getMaterialData().mIsSkinned;
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

    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        updateBoneTransforms();
    }
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

	setIndicesBuffer(Ptr<const GPUMesh>::cast(mInternalMesh));
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

void MeshBatcher::submitDataToGPU()
{	
    PROFILER_CPU()
    if(!mDataSubmittedToGPU)
    {
        if(!mBatchData.mIsInstanced)
        {
            setMeshBuffers(Ptr<const GPUMesh>::cast(mInternalMesh));
        }
        setInstancedBuffers();
        mDataSubmittedToGPU = true;
    }
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

    GPUVertexBufferData bufferDataObjectIDs(GPUBuiltIn::VertexInput::mObjectID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUVertexBuffersContainer.createVertexBuffer(bufferDataObjectIDs, isStatic);

    GPUVertexBufferData bufferDataMaterialInstanceIDs(GPUBuiltIn::VertexInput::mMaterialInstanceID, mBatchData.mIsInstanced ? 1 : 0);
    mGPUVertexBuffersContainer.createVertexBuffer(bufferDataMaterialInstanceIDs, isStatic);
    
    mGPUVertexBuffersContainer.disable();

    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        u32 bindingPointBoneMatrices = GET_SYSTEM(RenderSharedContext).requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mBonesMatrices.mType);
        mGPUSharedBuffersContainer.createSharedBuffer(bindingPointBoneMatrices, GPUBuiltIn::SharedBuffers::mBonesMatrices, isStatic);
        mGPUSharedBuffersContainer.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices).resize<Matrix4>(GPUBuiltIn::MAX_BONES);
    }
}

void MeshBatcher::resizeMeshBuffers(u32 maxInstances)
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
    u32 bufferSizeMultiplier = mBatchData.mIsInstanced ? 1 : mBatchData.mMesh->mVertexCount;
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).resize(maxInstances * bufferSizeMultiplier);
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mObjectID).resize(maxInstances * bufferSizeMultiplier);
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mMaterialInstanceID).resize(maxInstances * bufferSizeMultiplier);
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
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mObjectID).setDataArray(mObjectIDs);
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mMaterialInstanceID).setDataArray(mMaterialInstanceIDs);
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