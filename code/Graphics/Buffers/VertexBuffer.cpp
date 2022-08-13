#include "Graphics/Buffers/VertexBuffer.hpp"


VertexBuffer::~VertexBuffer() 
{
}

void VertexBuffer::init(u32 elementSize, u32 propertyArrayIndex, bool isStatic)
{
	terminate();

	mIsStatic = isStatic;
	mAttribPointerIndex = propertyArrayIndex;
	mVBO = RenderContext::createVBO(elementSize, mAttribPointerIndex);
	mGenerated = true;
}

void VertexBuffer::terminate()
{
	if(mGenerated)
	{
		glDeleteBuffers(1, &mVBO);
		mGenerated = false;
	}
}

void VertexBuffer::resize(u32 size)
{
	RenderContext::resizeVBO(mVBO, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

void VertexBuffer::setData(CR(std::vector<f32>)data)
{
	RenderContext::setDataVBO(mVBO, data);
}

BonesBuffer::~BonesBuffer()
{
}

void BonesBuffer::init(u32 propertyArrayIndex, bool isStatic)
{
	mIsStatic = isStatic;
	mAttribPointerIndex = propertyArrayIndex;

	mVBO = RenderContext::createVBOAnyType(1, GL_INT, sizeof(BoneVertexData), mAttribPointerIndex);

	RenderContext::enableProperty(mAttribPointerIndex + 1);
	glVertexAttribPointer(mAttribPointerIndex + 1, BoneVertexData::smMaxBonesPerVertex, GL_FLOAT, GL_FALSE, sizeof(BoneVertexData), (byte *)(BoneVertexData::smMaxBonesPerVertex * sizeof(i32)));

	mGenerated = true;
}

void BonesBuffer::terminate()
{
	if(mGenerated)
	{
		glDeleteBuffers(1, &mVBO);
		mGenerated = false;
	}
}

void BonesBuffer::resize(u32 size)
{
	RenderContext::resizeVBOAnyType(mVBO, sizeof(BoneVertexData), size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

void BonesBuffer::setData(CR(std::vector<BoneVertexData>)data)
{
	RenderContext::setDataVBOAnyType(mVBO, data);
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
	mVBOPosition.init(Mesh::smVertexPositionSize, 0, mIsStatic);
	mVBOTexture.init(Mesh::smVertexTexCoordSize, 1, mIsStatic);
	mVBOColor.init(Mesh::smVertexColorSize, 2, mIsStatic);
	mVBOBones.init(3, mIsStatic);

	if(mIsInstanced)
	{
		glGenBuffers(1, &mVBOMatrices);
		glBindBuffer(GL_ARRAY_BUFFER, mVBOMatrices);

		u32 columnBytesSize = Matrix4::smColumnSize * sizeof(f32);
		RenderContext::enableProperty(5);
		glVertexAttribPointer(5, Matrix4::smColumnSize, GL_FLOAT, GL_FALSE, 4 * columnBytesSize, (void*)0);
		RenderContext::enableProperty(6);
		glVertexAttribPointer(6, Matrix4::smColumnSize, GL_FLOAT, GL_FALSE, 4 * columnBytesSize, (void*)(1 * columnBytesSize));
		RenderContext::enableProperty(7);
		glVertexAttribPointer(7, Matrix4::smColumnSize, GL_FLOAT, GL_FALSE, 4 * columnBytesSize, (void*)(2 * columnBytesSize));
		RenderContext::enableProperty(8);
		glVertexAttribPointer(8, Matrix4::smColumnSize, GL_FLOAT, GL_FALSE, 4 * columnBytesSize, (void*)(3 * columnBytesSize));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
	}

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
		mVBOBones.terminate();

		if(mIsInstanced)
		{
			glDeleteBuffers(1, &mVBOMatrices);
		}

		glDeleteBuffers(1, &mEBO);

		mGenerated = false;
	}
}

void MeshBuffer::resize(CR(Mesh) mesh)
{
	mVBOPosition.resize(mesh.getVertices().capacity());
	mVBOTexture.resize(mesh.getTextureCoordinates().capacity());
	mVBOColor.resize(mesh.getColors().capacity());
	mVBOBones.resize(mesh.getBonesVertexData().capacity());

	if(mIsInstanced)
	{
		RenderContext::resizeVBO(mVBOMatrices, mMatrices.capacity(), mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}
}

void MeshBuffer::setData(CR(Mesh) mesh)
{
	mVBOPosition.setData(mesh.getVertices());
	mVBOTexture.setData(mesh.getTextureCoordinates());
	mVBOColor.setData(mesh.getColors());
	mVBOBones.setData(mesh.getBonesVertexData());

	if(mIsInstanced)
	{
		RenderContext::setDataVBO(mVBOMatrices, mMatrices);
	}
}

void MeshBuffer::setIndexesData(CR(Mesh) mesh)
{
	RenderContext::resizeEBO(mEBO, mesh.getFaces().size(), mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	RenderContext::setDataEBO(mEBO, mesh.getFaces());
}

void MeshBuffer::addInstanceMatrix(CR(Matrix4) modelMatrix)
{
	PROFILER_CPU()

	if(mIsInstanced)
	{
		std::copy(modelMatrix.getData(), modelMatrix.getData() + Matrix4::smMatrixSize, back_inserter(mMatrices));
	}
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
		mMatrices.reserve(Matrix4::smMatrixSize * maxInstances);
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

		u32 meshesAmount = mMeshBuffer.getIsInstanced() ? 1 : mMaxMeshesThreshold; 

		mMeshBuilder.init(mPrototypeMesh.get().getVertexCount() * meshesAmount, mPrototypeMesh.get().getFacesCount() * meshesAmount);
		//mMeshBuilder.copyBones(mPrototypeMesh);

		mMeshBuffer.setMaxInstances(meshesAmount);

		generateFacesData(meshesAmount);
	}

	if(mMeshBuffer.getIsInstanced())
	{
		mMeshBuilder.copyVertices(mPrototypeMesh);
		mMeshBuilder.copyTextureCoordinates(mPrototypeMesh);

		FOR_RANGE(i, 0, mPrototypeMesh.get().getVertexCount())
		{
			mMeshBuilder.addColor(0,0,0,1);
		}
	}

	mMeshBuffer.resize(mMeshBuilder);

	mMeshesIndex = 0;

	mDataSentToGPU = false;
}

void MeshBatcher::addInstanceMatrix(CR(Matrix4) modelMatrix)
{
	PROFILER_CPU()

	mMeshBuffer.addInstanceMatrix(modelMatrix);

	mMeshesIndex++;
}

void MeshBatcher::addInstance(CR(Mesh) meshInstance)
{
	PROFILER_CPU()

	mMeshBuilder.addVertices(meshInstance.getVertices());
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
		RenderContext::drawElements(mPrototypeMesh.get().getFaces().size(), mMeshesIndex, mMeshBuffer.getIsInstanced());
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
	mMeshBuilder.clear();
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
			mMeshBuilder.addFaceIndex(mPrototypeMesh.get().getFaces()[faceIndex] + offset);
		}
	}

	mMeshBuffer.setIndexesData(mMeshBuilder);
}

void MeshBatcher::sendDataToGPU()
{	
	if(!mDataSentToGPU)
	{
		mMeshBuffer.setData(mMeshBuilder);
		mDataSentToGPU = true;
	}
}
