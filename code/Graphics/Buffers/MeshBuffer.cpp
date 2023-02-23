#include "Graphics/Buffers/MeshBuffer.hpp"

MeshBuffer::~MeshBuffer() 
{
    terminate();
}

void MeshBuffer::init(bool isStatic, bool isInstanced)
{
	PROFILER_CPU()

	mIsStatic = isStatic;
	mIsInstanced = isInstanced;
	mVAO = GET_SYSTEM(RenderContext).createVAO();

    mBuffersLayout.init(mIsStatic || mIsInstanced);
    mVBOPosition = mBuffersLayout.addBuffer(sizeof(Vector3));
    mBuffersLayout.getBuffer(mVBOPosition).attribute(GPUBufferPrimitiveType::FLOAT);
    mVBOTexture = mBuffersLayout.addBuffer(sizeof(Vector2));
    mBuffersLayout.getBuffer(mVBOTexture).attribute(GPUBufferPrimitiveType::FLOAT);
    mVBOColor = mBuffersLayout.addBuffer(sizeof(Vector4));
    mBuffersLayout.getBuffer(mVBOColor).attribute(GPUBufferPrimitiveType::FLOAT);

	if(mIsInstanced)
	{
		u32 columnBytesSize = Matrix4::smColumnSize * sizeof(f32);

        mVBOModelMatrix = mBuffersLayout.addBuffer(sizeof(Matrix4));
        mBuffersLayout.getBuffer(mVBOModelMatrix).setInstanceDivisor(1);
		mBuffersLayout.getBuffer(mVBOModelMatrix).attribute(GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize);
		mBuffersLayout.getBuffer(mVBOModelMatrix).attribute(GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize);
		mBuffersLayout.getBuffer(mVBOModelMatrix).attribute(GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize);
		mBuffersLayout.getBuffer(mVBOModelMatrix).attribute(GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize);
	}

    mVBOBone = mBuffersLayout.addBuffer(sizeof(BoneVertexData));
	mBuffersLayout.getBuffer(mVBOBone).attribute(GPUBufferPrimitiveType::INT, BoneVertexData::smMaxBonesPerVertex);
	mBuffersLayout.getBuffer(mVBOBone).attribute(GPUBufferPrimitiveType::FLOAT, BoneVertexData::smMaxBonesPerVertex);

	mEBO = GET_SYSTEM(RenderContext).createEBO();
}

void MeshBuffer::terminate()
{
    disable();
    GET_SYSTEM(RenderContext).deleteVAO(mVAO);
    GET_SYSTEM(RenderContext).deleteEBO(mEBO);
}

void MeshBuffer::resize(const Mesh& mesh)
{
	mBuffersLayout.getBuffer(mVBOPosition).resize(mesh.mPositions.capacity());
	mBuffersLayout.getBuffer(mVBOTexture).resize(mesh.mTextureCoordinates.capacity());
	mBuffersLayout.getBuffer(mVBOColor).resize(mesh.mColors.capacity());
	mBuffersLayout.getBuffer(mVBOBone).resize(mesh.mBonesVertexData.capacity());
	
	if(mIsInstanced)
	{
		mBuffersLayout.getBuffer(mVBOModelMatrix).resize(mMatrices.capacity());
	}
}

void MeshBuffer::setData(const Mesh& mesh)
{
	mBuffersLayout.getBuffer(mVBOPosition).setData(mesh.mPositions);
	mBuffersLayout.getBuffer(mVBOTexture).setData(mesh.mTextureCoordinates);
	mBuffersLayout.getBuffer(mVBOColor).setData(mesh.mColors);
	mBuffersLayout.getBuffer(mVBOBone).setData(mesh.mBonesVertexData);
}

void MeshBuffer::setIndexesData(const Mesh& mesh)
{
	GET_SYSTEM(RenderContext).resizeEBO(mEBO, mesh.mFaces.size() * 3, mIsStatic || mIsInstanced ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	GET_SYSTEM(RenderContext).setDataEBO(mEBO, mesh.mFaces);
}

void MeshBuffer::addInstanceMatrix(const Matrix4& modelMatrix)
{
	PROFILER_CPU()

	if(mIsInstanced)
	{
		mMatrices.push_back(modelMatrix);
	}
}

void MeshBuffer::setDataInstanced()
{
	mBuffersLayout.getBuffer(mVBOModelMatrix).setData(mMatrices);
}

void MeshBuffer::clear()
{
	PROFILER_CPU()

	if(mIsInstanced)
	{
		mMatrices.clear();
	}
}

void MeshBuffer::setMaxInstances(u32 maxInstances)
{
	PROFILER_CPU()

	if(mIsInstanced)
	{
		mMatrices.reserve(maxInstances);
		mBuffersLayout.getBuffer(mVBOModelMatrix).resize(mMatrices.capacity());
	}
}

void MeshBuffer::enable()
{
	GET_SYSTEM(RenderContext).enableVAO(mVAO);
}

void MeshBuffer::disable()
{
	GET_SYSTEM(RenderContext).enableVAO(0);
}