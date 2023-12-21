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
	mVertexBufferLayout = GET_SYSTEM(GPUInterface).createVertexBufferLayout();

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

    GPUVertexBufferData bufferNormal(GPUBuiltIn::VertexInput::mNormal);
    mVBONormal = mGPUVertexBuffersLayout.addBuffer(bufferNormal);

    GPUVertexBufferData bufferDataBonesIDs(GPUBuiltIn::VertexInput::mBonesIDs);
    mVBOBonesIDs = mGPUVertexBuffersLayout.addBuffer(bufferDataBonesIDs);

    GPUVertexBufferData bufferDataBonesWeights(GPUBuiltIn::VertexInput::mBonesWeights);
    mVBOBonesWeights = mGPUVertexBuffersLayout.addBuffer(bufferDataBonesWeights);

    GPUVertexBufferData bufferDataInstanceIDs(GPUBuiltIn::VertexInput::mInstanceID);
	if(mGPUMeshBufferData.mIsInstanced)
	{
        bufferDataInstanceIDs.mInstanceDivisor = 1;
	}
    mVBOInstanceIDs = mGPUVertexBuffersLayout.addBuffer(bufferDataInstanceIDs);

	mEBO = GET_SYSTEM(GPUInterface).createBuffer();

    u32 modelMatricesBindingPoint = GET_SYSTEM(GPUSharedContext).requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mModelMatrices.mType);
    mModelMatricesBuffer.init(modelMatricesBindingPoint, GPUBuiltIn::SharedBuffers::mModelMatrices, mGPUMeshBufferData.mIsStatic);

    u32 bonesMatricesBindingPoint = GET_SYSTEM(GPUSharedContext).requestSharedBufferBindingPoint(GPUBuiltIn::SharedBuffers::mBonesMatrices.mType);
    mBonesMatricesBuffer.init(bonesMatricesBindingPoint, GPUBuiltIn::SharedBuffers::mBonesMatrices, mGPUMeshBufferData.mIsStatic);
    mBonesMatricesBuffer.resize(Mesh::MAX_BONES);
}

void GPUMeshBuffer::terminate()
{
    disable();
    GET_SYSTEM(GPUInterface).deleteVertexBufferLayout(mVertexBufferLayout);
    GET_SYSTEM(GPUInterface).deleteBuffer(mEBO);
    mModelMatricesBuffer.terminate();
    mBonesMatricesBuffer.terminate();
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
	mGPUVertexBuffersLayout.getBuffer(mVBONormal).resize(mesh.mNormals.capacity());
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
    //CHECK_MSG(mMaxInstances <= 1024, "Max matrices reached (+1024)");
    mModelMatricesBuffer.resize(mMaxInstances);
}

void GPUMeshBuffer::setMeshData(const Mesh& mesh)
{
    PROFILER_CPU()
	mGPUVertexBuffersLayout.getBuffer(mVBOPosition).setDataArray(mesh.mPositions);
	mGPUVertexBuffersLayout.getBuffer(mVBOTexture).setDataArray(mesh.mTextureCoordinates);
    if(mGPUMeshBufferData.mUseVertexColor)
    {
	    mGPUVertexBuffersLayout.getBuffer(mVBOColor).setDataArray(mesh.mColors);
    }
	mGPUVertexBuffersLayout.getBuffer(mVBONormal).setDataArray(mesh.mNormals);
	mGPUVertexBuffersLayout.getBuffer(mVBOBonesIDs).setDataArray(mesh.mBonesVertexIDsData);
	mGPUVertexBuffersLayout.getBuffer(mVBOBonesWeights).setDataArray(mesh.mBonesVertexWeightsData);
}

void GPUMeshBuffer::setInstancesData(const std::vector<Matrix4>& matrices, const std::vector<u32>& instanceIDs)
{
    PROFILER_CPU()
    // CHECK_MSG(matrices.size() <= 1024, "Max matrices reached (+1024)");
    PROFILER_BLOCK_CPU("VBO instanceIDs");
	mGPUVertexBuffersLayout.getBuffer(mVBOInstanceIDs).setDataArray(instanceIDs);
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

void GPUMeshBuffer::setIndexesData(const Mesh& mesh)
{
    PROFILER_CPU()
	GET_SYSTEM(GPUInterface).resizeBuffer(GPUBufferType::INDEX, mEBO, sizeof(Face), mesh.mFaces.size(), mGPUMeshBufferData.mIsStatic || mGPUMeshBufferData.mIsInstanced);
	GET_SYSTEM(GPUInterface).setBufferDataArray(GPUBufferType::INDEX, mEBO, mesh.mFaces);
}

void GPUMeshBuffer::enable()
{
	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(mVertexBufferLayout);
}

void GPUMeshBuffer::disable()
{
	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(0);
}