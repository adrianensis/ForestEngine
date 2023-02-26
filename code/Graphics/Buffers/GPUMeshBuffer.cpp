#include "Graphics/Buffers/GPUMeshBuffer.hpp"
#include "Graphics/Mesh/Mesh.hpp"

GPUMeshBuffer::~GPUMeshBuffer() 
{
    terminate();
}

void GPUMeshBuffer::init(bool isStatic, bool isInstanced)
{
	PROFILER_CPU()

	mIsStatic = isStatic;
	mIsInstanced = isInstanced;
	mVAO = GET_SYSTEM(RenderContext).createVAO();

    mBuffersLayout.init(mIsStatic || mIsInstanced);
    GPUBufferData bufferDataPosition("position",sizeof(Vector3));
    mVBOPosition = mBuffersLayout.addBuffer(bufferDataPosition);
    GPUBufferData bufferDataTexture("texcoord",sizeof(Vector2));
    mVBOTexture = mBuffersLayout.addBuffer(bufferDataTexture);
    GPUBufferData bufferDataColor("color",sizeof(Vector4));
    mVBOColor = mBuffersLayout.addBuffer(bufferDataColor);

	if(mIsInstanced)
	{
        GPUBufferData bufferDataMatrix("modelMatrix",sizeof(Matrix4));
        bufferDataMatrix.mInstanceDivisor = 1;
        bufferDataMatrix.mAttributeDivisorSizeInPrimitiveTypes = Matrix4::smColumnSize;

        mVBOModelMatrix = mBuffersLayout.addBuffer(bufferDataMatrix);
	}

    GPUBufferData bufferDataBonesIDs("BoneIDs",sizeof(BoneVertexIDsData));
    bufferDataBonesIDs.mPrimitiveType = GPUBufferPrimitiveType::INT;
    mVBOBonesIDs = mBuffersLayout.addBuffer(bufferDataBonesIDs);

    GPUBufferData bufferDataBonesWeights("Weights",sizeof(BoneVertexIDsData));
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
	mBuffersLayout.getBuffer(mVBOColor).resize(mesh.mColors.capacity());
	mBuffersLayout.getBuffer(mVBOBonesIDs).resize(mesh.mBonesVertexIDsData.capacity());
	mBuffersLayout.getBuffer(mVBOBonesWeights).resize(mesh.mBonesVertexWeightsData.capacity());
	
	if(mIsInstanced)
	{
		mBuffersLayout.getBuffer(mVBOModelMatrix).resize(mMatrices.capacity());
	}
}

void GPUMeshBuffer::setData(const Mesh& mesh)
{
	mBuffersLayout.getBuffer(mVBOPosition).setData(mesh.mPositions);
	mBuffersLayout.getBuffer(mVBOTexture).setData(mesh.mTextureCoordinates);
	mBuffersLayout.getBuffer(mVBOColor).setData(mesh.mColors);
	mBuffersLayout.getBuffer(mVBOBonesIDs).setData(mesh.mBonesVertexIDsData);
	mBuffersLayout.getBuffer(mVBOBonesWeights).setData(mesh.mBonesVertexWeightsData);
}

void GPUMeshBuffer::setIndexesData(const Mesh& mesh)
{
	GET_SYSTEM(RenderContext).resizeEBO(mEBO, mesh.mFaces.size() * 3, mIsStatic || mIsInstanced ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	GET_SYSTEM(RenderContext).setDataEBO(mEBO, mesh.mFaces);
}

void GPUMeshBuffer::addInstanceMatrix(const Matrix4& modelMatrix)
{
	PROFILER_CPU()

	if(mIsInstanced)
	{
		mMatrices.push_back(modelMatrix);
	}
}

void GPUMeshBuffer::setDataInstanced()
{
	mBuffersLayout.getBuffer(mVBOModelMatrix).setData(mMatrices);
}

void GPUMeshBuffer::clear()
{
	PROFILER_CPU()

	if(mIsInstanced)
	{
		mMatrices.clear();
	}
}

void GPUMeshBuffer::setMaxInstances(u32 maxInstances)
{
	PROFILER_CPU()

	if(mIsInstanced)
	{
		mMatrices.reserve(maxInstances);
		mBuffersLayout.getBuffer(mVBOModelMatrix).resize(mMatrices.capacity());
	}
}

void GPUMeshBuffer::enable()
{
	GET_SYSTEM(RenderContext).enableVAO(mVAO);
}

void GPUMeshBuffer::disable()
{
	GET_SYSTEM(RenderContext).enableVAO(0);
}