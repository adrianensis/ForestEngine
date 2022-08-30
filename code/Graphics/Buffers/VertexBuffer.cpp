#include "Graphics/Buffers/VertexBuffer.hpp"


GPUBufferBase::~GPUBufferBase() 
{
}

void GPUBufferBase::init(u32 typeSizeInBytes, bool isStatic)
{
	terminate();

	mTypeSizeInBytes = typeSizeInBytes;
	mIsStatic = isStatic;

	mVBO = RenderContext::createVBO();

	mGenerated = true;
}

void GPUBufferBase::terminate()
{
	if(mGenerated)
	{
		glDeleteBuffers(1, &mVBO);
		mGenerated = false;
	}
}

void GPUBufferBase::resize(u32 size)
{
	RenderContext::resizeVBOAnyType(mVBO, mTypeSizeInBytes, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

void GPUBufferBase::attribute(u32 attributeIndex, GPUBufferPrimitiveType primitiveType, u32 pointerOffset, u32 divisor)
{
	u32 primitiveTypeSize = mTypeSizeInBytes;
	switch (primitiveType)
	{
		case GPUBufferPrimitiveType::FLOAT:
			primitiveTypeSize = sizeof(f32);
		break;
		case GPUBufferPrimitiveType::INT:
			primitiveTypeSize = sizeof(u32);
		break;
	}

	attributeCustomSize(attributeIndex, primitiveType, mTypeSizeInBytes/primitiveTypeSize, pointerOffset, divisor);
}

void GPUBufferBase::attributeCustomSize(u32 attributeIndex, GPUBufferPrimitiveType primitiveType, u32 elementSize, u32 pointerOffset, u32 divisor)
{
	RenderContext::attribute(attributeIndex, elementSize, (u32)primitiveType, mTypeSizeInBytes, pointerOffset, divisor);
}

MeshBuffer::~MeshBuffer() 
{
}

void MeshBuffer::init(bool isStatic, bool isInstanced)
{
	PROFILER_CPU()

	terminate();

	mIsStatic = isStatic;
	mIsInstanced = isInstanced;
	mVAO = RenderContext::createVAO();
	// Force static draw if instanced
	mVBOPosition.init(mIsStatic || mIsInstanced);
	mVBOPosition.attribute(0, GPUBufferPrimitiveType::FLOAT, 0, 0);
	mVBOTexture.init(mIsStatic || mIsInstanced);
	mVBOTexture.attribute(1, GPUBufferPrimitiveType::FLOAT, 0, 0);
	mVBOColor.init(mIsStatic || mIsInstanced);
	mVBOColor.attribute(2, GPUBufferPrimitiveType::FLOAT, 0, 0);

	if(mIsInstanced)
	{
		mVBOModelMatrix.init(mIsStatic || mIsInstanced);

		u32 columnBytesSize = Matrix4::smColumnSize * sizeof(f32);

		mVBOModelMatrix.attributeCustomSize(3, GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 0 * columnBytesSize, 1);
		mVBOModelMatrix.attributeCustomSize(4, GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 1 * columnBytesSize, 1);
		mVBOModelMatrix.attributeCustomSize(5, GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 2 * columnBytesSize, 1);
		mVBOModelMatrix.attributeCustomSize(6, GPUBufferPrimitiveType::FLOAT, Matrix4::smColumnSize, 3 * columnBytesSize, 1);
	}

	mVBOBone.init(mIsStatic || mIsInstanced);
	mVBOBone.attributeCustomSize(7, GPUBufferPrimitiveType::INT, BoneVertexData::smMaxBonesPerVertex, 0, 0);
	mVBOBone.attributeCustomSize(8, GPUBufferPrimitiveType::FLOAT, BoneVertexData::smMaxBonesPerVertex, BoneVertexData::smMaxBonesPerVertex * sizeof(i32), 0);

	mEBO = RenderContext::createEBO();

	mGenerated = true;
}

void MeshBuffer::terminate()
{
	if(mGenerated)
	{
		glDeleteVertexArrays(1, &mVAO);
		mVBOPosition.terminate();
		mVBOTexture.terminate();
		mVBOColor.terminate();
		mVBOBone.terminate();

		if(mIsInstanced)
		{
			mVBOModelMatrix.terminate();
		}

		glDeleteBuffers(1, &mEBO);

		mGenerated = false;
	}
}

void MeshBuffer::resize(const Mesh& mesh)
{
	mVBOPosition.resize(mesh.getPositions().capacity());
	mVBOTexture.resize(mesh.getTextureCoordinates().capacity());
	mVBOColor.resize(mesh.getColors().capacity());
	mVBOBone.resize(mesh.getBonesVertexData().capacity());
	
	if(mIsInstanced)
	{
		mVBOModelMatrix.resize(mMatrices.capacity());
	}
}

void MeshBuffer::setData(const Mesh& mesh)
{
	mVBOPosition.setData(mesh.getPositions());
	mVBOTexture.setData(mesh.getTextureCoordinates());
	mVBOColor.setData(mesh.getColors());
	mVBOBone.setData(mesh.getBonesVertexData());
}

void MeshBuffer::setIndexesData(const Mesh& mesh)
{
	RenderContext::resizeEBO(mEBO, mesh.getFaces().size() * 3, mIsStatic || mIsInstanced ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	RenderContext::setDataEBO(mEBO, mesh.getFaces());
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

MeshBatcher::~MeshBatcher() 
{
}

void MeshBatcher::init(Ptr<const Mesh> prototypeMesh, bool isStatic, bool isInstanced)
{
	PROFILER_CPU()

	terminate();
	mMeshBuffer.init(isStatic, isInstanced);
	mPrototypeMesh = prototypeMesh;
	
	if(mMeshBuffer.getIsInstanced())
	{
		mMeshBuilder.init(mPrototypeMesh.get().getVertexCount() * 1, mPrototypeMesh.get().getFacesCount() * 1);

		mMeshBuilder.addPositions(mPrototypeMesh.get().getPositions());
		mMeshBuilder.addTextureCoordinates(mPrototypeMesh.get().getTextureCoordinates());
		FOR_RANGE(i, 0, mPrototypeMesh.get().getVertexCount())
		{
			mMeshBuilder.addColor(Vector4(0,0,0,1));
		}
		mMeshBuilder.addBonesVertexData(mPrototypeMesh.get().getBonesVertexData());

		generateFacesData(1);
		mMeshBuffer.resize(mMeshBuilder);

		mDataSentToGPU = false;

		mMeshBuffer.setData(mMeshBuilder);

		mMeshesIndex = 0;
	}
}

void MeshBatcher::resize(u32 size)
{
	PROFILER_CPU()

	clear();

	if (size > mMaxMeshesThreshold)
	{
		if(mMaxMeshesThreshold == 0)
		{
			mMaxMeshesThreshold = size;
		}
		else
		{
			mMaxMeshesThreshold += mMaxMeshesIncrement;
		}

		if (mMeshBuffer.getIsInstanced())
		{
			mMeshBuffer.setMaxInstances(mMaxMeshesThreshold);
		}
		else
		{
			mMeshBuilder.init(mPrototypeMesh.get().getVertexCount() * mMaxMeshesThreshold, mPrototypeMesh.get().getFacesCount() * mMaxMeshesThreshold);
			generateFacesData(mMaxMeshesThreshold);
			mMeshBuffer.resize(mMeshBuilder);
		}
	}

	mMeshesIndex = 0;

	mDataSentToGPU = false;
}

void MeshBatcher::addInstanceMatrix(const Matrix4& modelMatrix)
{
	PROFILER_CPU()

	mMeshBuffer.addInstanceMatrix(modelMatrix);

	mMeshesIndex++;
}

void MeshBatcher::addInstance(const Mesh& meshInstance)
{
	PROFILER_CPU()

	mMeshBuilder.addPositions(meshInstance.getPositions());
	mMeshBuilder.addTextureCoordinates(meshInstance.getTextureCoordinates());
	mMeshBuilder.addColors(meshInstance.getColors());
	mMeshBuilder.addBonesVertexData(meshInstance.getBonesVertexData());

	mMeshesIndex++;
}

void MeshBatcher::drawCall()
{	
	if (mMeshesIndex > 0)
	{
		sendDataToGPU();
		RenderContext::drawElements(mPrototypeMesh.get().getFaces().size() * 3, mMeshesIndex, mMeshBuffer.getIsInstanced());
	}
}

void MeshBatcher::terminate()
{
	PROFILER_CPU()
	mMeshBuffer.terminate();
}

void MeshBatcher::enable()
{
	mMeshBuffer.enable();
}

void MeshBatcher::disable()
{
	mMeshBuffer.disable();
}

void MeshBatcher::clear()
{
	PROFILER_CPU()
	if( ! mMeshBuffer.getIsInstanced())
	{
		mMeshBuilder.clear();
	}
	mMeshBuffer.clear();
}

void MeshBatcher::generateFacesData(u32 meshesCount)
{
	PROFILER_CPU()

	FOR_RANGE(i, 0, meshesCount)
	{
		u32 offset = (i * mPrototypeMesh.get().getVertexCount());
		
		FOR_RANGE(faceIndex, 0, mPrototypeMesh.get().getFaces().size())
		{
			Face newFace = mPrototypeMesh.get().getFaces()[faceIndex];
			newFace.mIndex0 += offset;
			newFace.mIndex1 += offset;
			newFace.mIndex2 += offset;

			mMeshBuilder.addFace(newFace);
		}
	}

	mMeshBuffer.setIndexesData(mMeshBuilder);
}

void MeshBatcher::sendDataToGPU()
{	
	if(!mDataSentToGPU)
	{
		if(mMeshBuffer.getIsInstanced())
		{
			mMeshBuffer.setDataInstanced();
		}
		else
		{
			mMeshBuffer.setData(mMeshBuilder);
		}

		mDataSentToGPU = true;
	}
}
