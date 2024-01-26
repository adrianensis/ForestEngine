#include "Graphics/GPU/GPUMeshBuffer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"

void GPUMeshBuffer::init(const GPUMeshBufferData& gpuMeshBufferData)
{
	PROFILER_CPU()

	mGPUMeshBufferData = gpuMeshBufferData;

    mGPUBuffersLayout.init(mGPUMeshBufferData.mIsStatic || mGPUMeshBufferData.mIsInstanced);
    mGPUBuffersLayout.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mFace);

    FOR_ARRAY(i, mGPUMeshBufferData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mGPUMeshBufferData.mMesh->mGPUVertexInputBuffers[i];
        GPUVertexBufferData bufferData(gpuVariableData);
        mGPUBuffersLayout.createVertexBuffer(bufferData);
    }

    GPUVertexBufferData bufferDataInstanceIDs(GPUBuiltIn::VertexInput::mInstanceID, mGPUMeshBufferData.mIsInstanced ? 1 : 0);
    mGPUBuffersLayout.createVertexBuffer(bufferDataInstanceIDs);
    
    mGPUBuffersLayout.disable();

    mGPUBuffersLayout.createInstanceBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices);

    if(mGPUMeshBufferData.mIsSkinned)
    {
        u32 bonesMatricesBindingPoint = GET_SYSTEM(GPUSharedContext).requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mBonesMatrices.mType);
        mBonesMatricesBuffer.init(bonesMatricesBindingPoint, GPUBuiltIn::SharedBuffers::mBonesMatrices, mGPUMeshBufferData.mIsStatic);
        mBonesMatricesBuffer.resize<Matrix4>(GPUMesh::MAX_BONES);
    }
}

void GPUMeshBuffer::terminate()
{
    disable();
    mGPUBuffersLayout.terminate();
    if(mGPUMeshBufferData.mIsSkinned)
    {
        mBonesMatricesBuffer.terminate();
    }
}

void GPUMeshBuffer::resizeMeshData(u32 maxInstances)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mGPUMeshBufferData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mGPUMeshBufferData.mMesh->mGPUVertexInputBuffers[i];
        mGPUBuffersLayout.getVertexBuffer(gpuVariableData).resize(mGPUMeshBufferData.mMesh->mVertexCount * maxInstances);
    }
}

void GPUMeshBuffer::resizeInstancesData(u32 maxInstances)
{
    PROFILER_CPU()
    mMaxInstances = maxInstances;
    u32 matricesBufferSizeMultiplier = mGPUMeshBufferData.mIsInstanced ? 1 : mGPUMeshBufferData.mMesh->mVertexCount;
    mGPUBuffersLayout.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).resize(mMaxInstances * matricesBufferSizeMultiplier);
    mGPUBuffersLayout.getInstanceBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).resize<Matrix4>(mMaxInstances);
}

void GPUMeshBuffer::setMeshData(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mesh->mGPUVertexInputBuffers[i];
        mGPUBuffersLayout.getVertexBuffer(gpuVariableData).setDataArray(mesh->mBuffers.at(gpuVariableData.mName));
    }
}

void GPUMeshBuffer::setInstancesData(const std::vector<Matrix4>& matrices, const std::vector<u32>& instanceIDs)
{
    PROFILER_CPU()
    PROFILER_BLOCK_CPU("VBO instanceIDs");
	mGPUBuffersLayout.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).setDataArray(instanceIDs);
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("UBO matrices");
    mGPUBuffersLayout.getInstanceBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).setDataArray(matrices);
    PROFILER_END_BLOCK();
}

void GPUMeshBuffer::setBonesTransforms(const std::vector<Matrix4>& transforms)
{
    PROFILER_BLOCK_CPU("UBO Bones Transforms");
    mBonesMatricesBuffer.setDataArray(transforms);
    PROFILER_END_BLOCK();
}

void GPUMeshBuffer::setIndicesData(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    mGPUBuffersLayout.getIndicesBuffer().resize(mesh->mIndices.size());
    mGPUBuffersLayout.getIndicesBuffer().setDataArray(mesh->mIndices);
}

void GPUMeshBuffer::enable()
{
	mGPUBuffersLayout.enable();
}

void GPUMeshBuffer::disable()
{
	mGPUBuffersLayout.disable();
}