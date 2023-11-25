#include "Graphics/GPU/GPUMeshBuffer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Mesh/Mesh.hpp"
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
	mVAO = GET_SYSTEM(GPUInterface).createVAO();

    mGPUVertexBuffersLayout.init(mGPUMeshBufferData.mIsStatic || mGPUMeshBufferData.mIsInstanced);
    GPUVertexBufferData bufferDataPosition(GPUBuiltIn::VertexInput::mPosition);
    mVBOPosition = mGPUVertexBuffersLayout.addBuffer(bufferDataPosition);
    GPUVertexBufferData bufferDataTexture(GPUBuiltIn::VertexInput::mTextureCoord);
    mVBOTexture = mGPUVertexBuffersLayout.addBuffer(bufferDataTexture);
    if(mGPUMeshBufferData.mUseVertexColor)
    {
        GPUVertexBufferData bufferDataColor(GPUBuiltIn::VertexInput::mColor);
        mVBOColor = mGPUVertexBuffersLayout.addBuffer(bufferDataColor);
    }

    GPUVertexBufferData bufferDataBonesIDs(GPUBuiltIn::VertexInput::mBonesIDs);
    mVBOBonesIDs = mGPUVertexBuffersLayout.addBuffer(bufferDataBonesIDs);

    GPUVertexBufferData bufferDataBonesWeights(GPUBuiltIn::VertexInput::mBonesWeights);
    mVBOBonesWeights = mGPUVertexBuffersLayout.addBuffer(bufferDataBonesWeights);

    GPUVertexBufferData bufferDataInstanceIDs(GPUBuiltIn::VertexInput::mInstanceID);
    bufferDataInstanceIDs.mAttributeDivisorSizeInPrimitiveTypes = 1;
	if(mGPUMeshBufferData.mIsInstanced)
	{
        bufferDataInstanceIDs.mInstanceDivisor = 1;
	}
    mVBOInstanceIDs = mGPUVertexBuffersLayout.addBuffer(bufferDataInstanceIDs);

	mEBO = GET_SYSTEM(GPUInterface).createEBO();

    u32 modelMatricesBindingPoint = GET_SYSTEM(GPUSharedContext).requestUniformBufferBindingPoint();
    mModelMatricesBlock.init(modelMatricesBindingPoint, GPUBuiltIn::UniformBlocks::mModelMatrices, mGPUMeshBufferData.mIsStatic);

    u32 bonesMatricesBindingPoint = GET_SYSTEM(GPUSharedContext).requestUniformBufferBindingPoint();
    mBonesMatricesBlock.init(bonesMatricesBindingPoint, GPUBuiltIn::UniformBlocks::mBonesMatrices, mGPUMeshBufferData.mIsStatic);
    mBonesMatricesBlock.resize(Mesh::MAX_BONES);
}

void GPUMeshBuffer::terminate()
{
    disable();
    GET_SYSTEM(GPUInterface).deleteVAO(mVAO);
    GET_SYSTEM(GPUInterface).deleteEBO(mEBO);
    mModelMatricesBlock.terminate();
    mBonesMatricesBlock.terminate();
}

void GPUMeshBuffer::resizeMeshData(const Mesh& mesh)
{
    PROFILER_CPU()
	mGPUVertexBuffersLayout.getBuffer(mVBOPosition).resize(mesh.mPositions.capacity());
	mGPUVertexBuffersLayout.getBuffer(mVBOTexture).resize(mesh.mTextureCoordinates.capacity());
    if(mGPUMeshBufferData.mUseVertexColor)
    {
	    mGPUVertexBuffersLayout.getBuffer(mVBOColor).resize(mesh.mColors.capacity());
    }
	mGPUVertexBuffersLayout.getBuffer(mVBOBonesIDs).resize(mesh.mBonesVertexIDsData.capacity());
	mGPUVertexBuffersLayout.getBuffer(mVBOBonesWeights).resize(mesh.mBonesVertexWeightsData.capacity());
}

void GPUMeshBuffer::resizeInstancesData(u32 maxInstances)
{
    PROFILER_CPU()
    mMaxInstances = maxInstances;
    u32 matricesBufferSizeMultiplier = mGPUMeshBufferData.mIsInstanced ? 1 : mGPUMeshBufferData.mVertexCount;
    mGPUVertexBuffersLayout.getBuffer(mVBOInstanceIDs).resize(mMaxInstances * matricesBufferSizeMultiplier);
    // TODO: 1024
    CHECK_MSG(mMaxInstances <= 1024, "Max matrices reached (+1024)");
    mModelMatricesBlock.resize(mMaxInstances);
}

void GPUMeshBuffer::setMeshData(const Mesh& mesh)
{
    PROFILER_CPU()
	mGPUVertexBuffersLayout.getBuffer(mVBOPosition).setData(mesh.mPositions);
	mGPUVertexBuffersLayout.getBuffer(mVBOTexture).setData(mesh.mTextureCoordinates);
    if(mGPUMeshBufferData.mUseVertexColor)
    {
	    mGPUVertexBuffersLayout.getBuffer(mVBOColor).setData(mesh.mColors);
    }
	mGPUVertexBuffersLayout.getBuffer(mVBOBonesIDs).setData(mesh.mBonesVertexIDsData);
	mGPUVertexBuffersLayout.getBuffer(mVBOBonesWeights).setData(mesh.mBonesVertexWeightsData);
}

void GPUMeshBuffer::setInstancesData(const std::vector<Matrix4>& matrices, const std::vector<u32>& instanceIDs)
{
    PROFILER_CPU()
    CHECK_MSG(matrices.size() <= 1024, "Max matrices reached (+1024)");
    PROFILER_BLOCK_CPU("VBO instanceIDs");
	mGPUVertexBuffersLayout.getBuffer(mVBOInstanceIDs).setData(instanceIDs);
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("UBO matrices");
    mModelMatricesBlock.setDataArray(matrices);
    PROFILER_END_BLOCK();
}

void GPUMeshBuffer::setBonesTransforms(const std::vector<Matrix4>& transforms)
{
    PROFILER_BLOCK_CPU("UBO Bones Transforms");
    mBonesMatricesBlock.setDataArray(transforms);
    PROFILER_END_BLOCK();
}

void GPUMeshBuffer::setIndexesData(const Mesh& mesh)
{
    PROFILER_CPU()
	GET_SYSTEM(GPUInterface).resizeEBO(mEBO, mesh.mFaces.size() * 3, mGPUMeshBufferData.mIsStatic || mGPUMeshBufferData.mIsInstanced ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	GET_SYSTEM(GPUInterface).setDataEBO(mEBO, mesh.mFaces);
}

void GPUMeshBuffer::enable()
{
	GET_SYSTEM(GPUInterface).enableVAO(mVAO);
}

void GPUMeshBuffer::disable()
{
	GET_SYSTEM(GPUInterface).enableVAO(0);
}