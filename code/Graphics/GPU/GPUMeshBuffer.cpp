#include "Graphics/GPU/GPUMeshBuffer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"

GPUMeshBuffer::~GPUMeshBuffer() 
{
    terminate();
}

void GPUMeshBuffer::init(const GPUMeshBufferData& gpuMeshBufferData)
{
	PROFILER_CPU()

	mGPUMeshBufferData = gpuMeshBufferData;

    mGPUVertexBuffersLayout.init(mGPUMeshBufferData.mIsStatic || mGPUMeshBufferData.mIsInstanced);
    mGPUVertexBuffersLayout.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mFace);

    FOR_ARRAY(i, mGPUMeshBufferData.mGPUVertexInputBuffers.mBuffers)
    {
        const GPUVariableData& gpuVariableData = mGPUMeshBufferData.mGPUVertexInputBuffers.mBuffers[i];
        GPUVertexBufferData bufferData(gpuVariableData);
        mGPUVertexBuffersLayout.createBuffer(bufferData);
    }

    // GPUVertexBufferData bufferDataPosition(GPUBuiltIn::VertexInput::mPosition);
    // mGPUVertexBuffersLayout.createBuffer(bufferDataPosition);
    // GPUVertexBufferData bufferDataTexture(GPUBuiltIn::VertexInput::mTextureCoord);
    // mGPUVertexBuffersLayout.createBuffer(bufferDataTexture);
    // if(mGPUMeshBufferData.mUseVertexColor)
    // {
    //     GPUVertexBufferData bufferDataColor(GPUBuiltIn::VertexInput::mColor);
    //     mGPUVertexBuffersLayout.createBuffer(bufferDataColor);
    // }

    // GPUVertexBufferData bufferNormal(GPUBuiltIn::VertexInput::mNormal);
    // mGPUVertexBuffersLayout.createBuffer(bufferNormal);

    // GPUVertexBufferData bufferDataBonesIDs(GPUBuiltIn::VertexInput::mBonesIDs);
    // mGPUVertexBuffersLayout.createBuffer(bufferDataBonesIDs);

    // GPUVertexBufferData bufferDataBonesWeights(GPUBuiltIn::VertexInput::mBonesWeights);
    // mGPUVertexBuffersLayout.createBuffer(bufferDataBonesWeights);

    GPUVertexBufferData bufferDataInstanceIDs(GPUBuiltIn::VertexInput::mInstanceID);
	if(mGPUMeshBufferData.mIsInstanced)
	{
        bufferDataInstanceIDs.mInstanceDivisor = 1;
	}
    mGPUVertexBuffersLayout.createBuffer(bufferDataInstanceIDs);

    u32 modelMatricesBindingPoint = GET_SYSTEM(GPUSharedContext).requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mModelMatrices.mType);
    mModelMatricesBuffer.init(modelMatricesBindingPoint, GPUBuiltIn::SharedBuffers::mModelMatrices, mGPUMeshBufferData.mIsStatic);

    u32 bonesMatricesBindingPoint = GET_SYSTEM(GPUSharedContext).requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mBonesMatrices.mType);
    mBonesMatricesBuffer.init(bonesMatricesBindingPoint, GPUBuiltIn::SharedBuffers::mBonesMatrices, mGPUMeshBufferData.mIsStatic);
    mBonesMatricesBuffer.resize(GPUMesh::MAX_BONES);

    mGPUVertexBuffersLayout.disable();
}

void GPUMeshBuffer::terminate()
{
    disable();
    mGPUVertexBuffersLayout.terminate();
    mModelMatricesBuffer.terminate();
    mBonesMatricesBuffer.terminate();
}

void GPUMeshBuffer::resizeMeshData(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mesh->mGPUVertexInputBuffers.mBuffers)
    {
        const GPUVariableData& gpuVariableData = mesh->mGPUVertexInputBuffers.mBuffers[i];
        mGPUVertexBuffersLayout.getBuffer(gpuVariableData).resize(mesh->mGPUMeshByteBuffers.mBuffers.at(gpuVariableData.mName).capacity());
    }
}

void GPUMeshBuffer::resizeInstancesData(u32 maxInstances)
{
    PROFILER_CPU()
    mMaxInstances = maxInstances;
    u32 matricesBufferSizeMultiplier = mGPUMeshBufferData.mIsInstanced ? 1 : mGPUMeshBufferData.mVertexCount;
    mGPUVertexBuffersLayout.getBuffer(GPUBuiltIn::VertexInput::mInstanceID).resize(mMaxInstances * matricesBufferSizeMultiplier);
    mModelMatricesBuffer.resize(mMaxInstances);
}

void GPUMeshBuffer::setMeshData(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mesh->mGPUVertexInputBuffers.mBuffers)
    {
        const GPUVariableData& gpuVariableData = mesh->mGPUVertexInputBuffers.mBuffers[i];
        mGPUVertexBuffersLayout.getBuffer(gpuVariableData).setDataArray(mesh->mGPUMeshByteBuffers.mBuffers.at(gpuVariableData.mName));
    }
}

void GPUMeshBuffer::setInstancesData(const std::vector<Matrix4>& matrices, const std::vector<u32>& instanceIDs)
{
    PROFILER_CPU()
    PROFILER_BLOCK_CPU("VBO instanceIDs");
	mGPUVertexBuffersLayout.getBuffer(GPUBuiltIn::VertexInput::mInstanceID).setDataArray(instanceIDs);
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("UBO matrices");
    mModelMatricesBuffer.setDataArray(matrices);
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
    mGPUVertexBuffersLayout.getIndicesBuffer().resize(mesh->mFaces.size());
    mGPUVertexBuffersLayout.getIndicesBuffer().setDataArray(mesh->mFaces);
}

void GPUMeshBuffer::enable()
{
	mGPUVertexBuffersLayout.enable();
}

void GPUMeshBuffer::disable()
{
	mGPUVertexBuffersLayout.disable();
}