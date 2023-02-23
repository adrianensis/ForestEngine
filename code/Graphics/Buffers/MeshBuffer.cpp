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

    u32 attributeIndex = 0;

	// Force static draw if instanced
	mVBOPosition.init(attributeIndex, sizeof(Vector3), mIsStatic || mIsInstanced);
	attributeIndex = mVBOPosition.attribute(GPUBufferPrimitiveType::FLOAT, 0, 0);
	mVBOTexture.init(attributeIndex, sizeof(Vector2), mIsStatic || mIsInstanced);
	attributeIndex = mVBOTexture.attribute(GPUBufferPrimitiveType::FLOAT, 0, 0);
	mVBOColor.init(attributeIndex, sizeof(Vector4), mIsStatic || mIsInstanced);
	attributeIndex = mVBOColor.attribute(GPUBufferPrimitiveType::FLOAT, 0, 0);

	if(mIsInstanced)
	{
		mVBOModelMatrix.init(attributeIndex, sizeof(Matrix4), mIsStatic || mIsInstanced);

		u32 columnBytesSize = Matrix4::smColumnSize * sizeof(f32);

		mVBOModelMatrix.attributeCustomSize(GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 0 * columnBytesSize, 1);
		mVBOModelMatrix.attributeCustomSize(GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 1 * columnBytesSize, 1);
		mVBOModelMatrix.attributeCustomSize(GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 2 * columnBytesSize, 1);
		attributeIndex = mVBOModelMatrix.attributeCustomSize(GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 3 * columnBytesSize, 1);
	}

	mVBOBone.init(attributeIndex, sizeof(BoneVertexData), mIsStatic || mIsInstanced);
	mVBOBone.attributeCustomSize(GPUBufferPrimitiveType::INT, BoneVertexData::smMaxBonesPerVertex, 0, 0);
	attributeIndex = mVBOBone.attributeCustomSize(GPUBufferPrimitiveType::FLOAT, BoneVertexData::smMaxBonesPerVertex, BoneVertexData::smMaxBonesPerVertex * sizeof(i32), 0);

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
	GET_SYSTEM(RenderContext).enableVAO(mVAO);
}

void MeshBuffer::disable()
{
	GET_SYSTEM(RenderContext).enableVAO(0);
}