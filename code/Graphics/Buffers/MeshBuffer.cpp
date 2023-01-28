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
	mVAO = RenderContext::createVAO();
	// Force static draw if instanced
	mVBOPosition.init(mIsStatic or mIsInstanced);
	mVBOPosition.attribute(0, GPUBufferPrimitiveType::FLOAT, 0, 0);
	mVBOTexture.init(mIsStatic or mIsInstanced);
	mVBOTexture.attribute(1, GPUBufferPrimitiveType::FLOAT, 0, 0);
	mVBOColor.init(mIsStatic or mIsInstanced);
	mVBOColor.attribute(2, GPUBufferPrimitiveType::FLOAT, 0, 0);

	if(mIsInstanced)
	{
		mVBOModelMatrix.init(mIsStatic or mIsInstanced);

		u32 columnBytesSize = Matrix4::smColumnSize * sizeof(f32);

		mVBOModelMatrix.attributeCustomSize(3, GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 0 * columnBytesSize, 1);
		mVBOModelMatrix.attributeCustomSize(4, GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 1 * columnBytesSize, 1);
		mVBOModelMatrix.attributeCustomSize(5, GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 2 * columnBytesSize, 1);
		mVBOModelMatrix.attributeCustomSize(6, GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 3 * columnBytesSize, 1);
	}

	mVBOBone.init(mIsStatic or mIsInstanced);
	mVBOBone.attributeCustomSize(7, GPUBufferPrimitiveType::INT, BoneVertexData::smMaxBonesPerVertex, 0, 0);
	mVBOBone.attributeCustomSize(8, GPUBufferPrimitiveType::FLOAT, BoneVertexData::smMaxBonesPerVertex, BoneVertexData::smMaxBonesPerVertex * sizeof(i32), 0);

	mEBO = RenderContext::createEBO();

	mGenerated = true;
}

void MeshBuffer::terminate()
{
	if(mGenerated)
	{
		RenderContext::deleteVAO(mVAO);
		RenderContext::deleteEBO(mEBO);
	}
}

void MeshBuffer::resize(const Mesh& mesh)
{
	mVBOPosition.resize(mesh.mPositions.capacity());
	mVBOTexture.resize(mesh.mTextureCoordinates.capacity());
	mVBOColor.resize(mesh.mColors.capacity());
	mVBOBone.resize(mesh.mBonesVertexData.capacity());
	
	if(mIsInstanced)
	{
		mVBOModelMatrix.resize(mMatrices.capacity());
	}
}

void MeshBuffer::setData(const Mesh& mesh)
{
	mVBOPosition.setData(mesh.mPositions);
	mVBOTexture.setData(mesh.mTextureCoordinates);
	mVBOColor.setData(mesh.mColors);
	mVBOBone.setData(mesh.mBonesVertexData);
}

void MeshBuffer::setIndexesData(const Mesh& mesh)
{
	RenderContext::resizeEBO(mEBO, mesh.mFaces.size() * 3, mIsStatic or mIsInstanced ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	RenderContext::setDataEBO(mEBO, mesh.mFaces);
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
	mVBOModelMatrix.setData(mMatrices);
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
		mVBOModelMatrix.resize(mMatrices.capacity());
	}
}

void MeshBuffer::enable()
{
	RenderContext::enableVAO(mVAO);
}

void MeshBuffer::disable()
{
	RenderContext::enableVAO(0);
}