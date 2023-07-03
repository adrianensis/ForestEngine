#include "Graphics/Buffers/GPUMeshBuffer.hpp"
#include "Graphics/Buffers/GPUDefinitions.hpp"
#include "Graphics/Mesh/Mesh.hpp"

GPUMeshBuffer::~GPUMeshBuffer() 
{
    terminate();
}

void GPUMeshBuffer::init(const GPUMeshBufferData& gpuMeshBufferData)
{
	PROFILER_CPU()

	mGPUMeshBufferData = gpuMeshBufferData;
	mVAO = GET_SYSTEM(RenderContext).createVAO();

    mBuffersLayout.init(mGPUMeshBufferData.mIsStatic || mGPUMeshBufferData.mIsInstanced);
    GPUBufferData bufferDataPosition(GPUBuiltIn::VertexInput::mPosition);
    mVBOPosition = mBuffersLayout.addBuffer(bufferDataPosition);
    GPUBufferData bufferDataTexture(GPUBuiltIn::VertexInput::mTexture);
    mVBOTexture = mBuffersLayout.addBuffer(bufferDataTexture);
    if(mGPUMeshBufferData.mUseVertexColor)
    {
        GPUBufferData bufferDataColor(GPUBuiltIn::VertexInput::mColor);
        mVBOColor = mBuffersLayout.addBuffer(bufferDataColor);
    }

    GPUBufferData bufferDataMatrix(GPUBuiltIn::VertexInput::mModelMatrix);
    bufferDataMatrix.mAttributeDivisorSizeInPrimitiveTypes = Matrix4::smColumnSize;
	if(mGPUMeshBufferData.mIsInstanced)
	{
        bufferDataMatrix.mInstanceDivisor = 1;
	}
    mVBOModelMatrix = mBuffersLayout.addBuffer(bufferDataMatrix);

    GPUBufferData bufferDataBonesIDs(GPUBuiltIn::VertexInput::mBonesIDs);
    mVBOBonesIDs = mBuffersLayout.addBuffer(bufferDataBonesIDs);

    GPUBufferData bufferDataBonesWeights(GPUBuiltIn::VertexInput::mBonesWeights);
    mVBOBonesWeights = mBuffersLayout.addBuffer(bufferDataBonesWeights);

	mEBO = GET_SYSTEM(RenderContext).createEBO();
}

void GPUMeshBuffer::terminate()
{
    disable();
    GET_SYSTEM(RenderContext).deleteVAO(mVAO);
    GET_SYSTEM(RenderContext).deleteEBO(mEBO);
}

void GPUMeshBuffer::resize(const Mesh& mesh)
{
	mBuffersLayout.getBuffer(mVBOPosition).resize(mesh.mPositions.capacity());
	mBuffersLayout.getBuffer(mVBOTexture).resize(mesh.mTextureCoordinates.capacity());
    if(mGPUMeshBufferData.mUseVertexColor)
    {
	    mBuffersLayout.getBuffer(mVBOColor).resize(mesh.mColors.capacity());
    }
	mBuffersLayout.getBuffer(mVBOBonesIDs).resize(mesh.mBonesVertexIDsData.capacity());
	mBuffersLayout.getBuffer(mVBOBonesWeights).resize(mesh.mBonesVertexWeightsData.capacity());
    mBuffersLayout.getBuffer(mVBOModelMatrix).resize(mMatrices.capacity());
}

void GPUMeshBuffer::setData(const Mesh& mesh)
{
	mBuffersLayout.getBuffer(mVBOPosition).setData(mesh.mPositions);
	mBuffersLayout.getBuffer(mVBOTexture).setData(mesh.mTextureCoordinates);
    if(mGPUMeshBufferData.mUseVertexColor)
    {
	    mBuffersLayout.getBuffer(mVBOColor).setData(mesh.mColors);
    }
	mBuffersLayout.getBuffer(mVBOBonesIDs).setData(mesh.mBonesVertexIDsData);
	mBuffersLayout.getBuffer(mVBOBonesWeights).setData(mesh.mBonesVertexWeightsData);
    mBuffersLayout.getBuffer(mVBOModelMatrix).setData(mMatrices);
}

void GPUMeshBuffer::setIndexesData(const Mesh& mesh)
{
	GET_SYSTEM(RenderContext).resizeEBO(mEBO, mesh.mFaces.size() * 3, mGPUMeshBufferData.mIsStatic || mGPUMeshBufferData.mIsInstanced ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	GET_SYSTEM(RenderContext).setDataEBO(mEBO, mesh.mFaces);
}

void GPUMeshBuffer::addInstanceMatrix(const Matrix4& modelMatrix)
{
	PROFILER_CPU()
    if(mGPUMeshBufferData.mIsInstanced)
	{
	    mMatrices.push_back(modelMatrix);
    }
    else
    {
        FOR_RANGE(i, 0, mGPUMeshBufferData.mVertexCount)
        {
            mMatrices.push_back(modelMatrix);
        }
    }
}

void GPUMeshBuffer::clear()
{
	PROFILER_CPU()

    mMatrices.clear();
}

void GPUMeshBuffer::setMaxInstances(u32 maxInstances)
{
	PROFILER_CPU()

    mMatrices.clear();
    u32 matricesBufferSizeMultiplier = mGPUMeshBufferData.mIsInstanced ? 1 : mGPUMeshBufferData.mVertexCount;
    mMatrices.reserve(maxInstances * matricesBufferSizeMultiplier);
    mBuffersLayout.getBuffer(mVBOModelMatrix).resize(mMatrices.capacity());
}

void GPUMeshBuffer::enable()
{
	GET_SYSTEM(RenderContext).enableVAO(mVAO);
}

void GPUMeshBuffer::disable()
{
	GET_SYSTEM(RenderContext).enableVAO(0);
}