#include "Graphics/Buffers/VertexBuffer.hpp"

VertexBuffer::~VertexBuffer()
{
	//terminate();
}

void VertexBuffer::init(u32 elementSize, u32 propertyArrayIndex, bool isStatic)
{
	terminate();

	mIsStatic = isStatic;
	mVBO = RenderContext::createVBO(elementSize, propertyArrayIndex);
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

void VertexBuffer::setData(const std::vector<f32> &data)
{
	RenderContext::setDataVBO(mVBO, data);
}

// ------------------------------------------------------
// ------------------------------------------------------

MeshBuffer::~MeshBuffer()
{
	//terminate();
}

void MeshBuffer::init(bool isStatic, bool isInstanced)
{
	terminate();

	mIsStatic = isStatic;
	mIsInstanced = isInstanced;
	mVAO = RenderContext::createVAO();
	mVBOPosition.init(Mesh::smVertexPositionSize, 0, mIsStatic);
	mVBOTexture.init(Mesh::smVertexTexCoordSize, 1, mIsStatic);
	mVBOColor.init(Mesh::smVertexColorSize, 2, mIsStatic);
	mEBO = RenderContext::createEBO();

	if(mIsInstanced)
	{
		glGenBuffers(1, &mVBOMatrices);

		glBindBuffer(GL_ARRAY_BUFFER, mVBOMatrices);

		// vertex attributes
		u32 columnBytesSize = Matrix4::smColumnSize * sizeof(f32);
		RenderContext::enableProperty(3);
		glVertexAttribPointer(3, Matrix4::smColumnSize, GL_FLOAT, GL_FALSE, 4 * columnBytesSize, (void*)0);
		RenderContext::enableProperty(4);
		glVertexAttribPointer(4, Matrix4::smColumnSize, GL_FLOAT, GL_FALSE, 4 * columnBytesSize, (void*)(1 * columnBytesSize));
		RenderContext::enableProperty(5);
		glVertexAttribPointer(5, Matrix4::smColumnSize, GL_FLOAT, GL_FALSE, 4 * columnBytesSize, (void*)(2 * columnBytesSize));
		RenderContext::enableProperty(6);
		glVertexAttribPointer(6, Matrix4::smColumnSize, GL_FLOAT, GL_FALSE, 4 * columnBytesSize, (void*)(3 * columnBytesSize));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
	}

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
		glDeleteBuffers(1, &mEBO);

		if(mIsInstanced)
		{
			glDeleteBuffers(1, &mVBOMatrices);
		}

		mGenerated = false;
	}
}

void MeshBuffer::resize(const Mesh& mesh)
{
	mVBOPosition.resize(mesh.getVertices().capacity());
	mVBOTexture.resize(mesh.getTextureCoordinates().capacity());
	mVBOColor.resize(mesh.getColors().capacity());

	if(mIsInstanced)
	{
		RenderContext::resizeVBO(mVBOMatrices, mMatrices.capacity(), mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}
}

void MeshBuffer::setData(const Mesh& mesh)
{
	mVBOPosition.setData(mesh.getVertices());
	mVBOTexture.setData(mesh.getTextureCoordinates());
	mVBOColor.setData(mesh.getColors());

	if(mIsInstanced)
	{
		RenderContext::setDataVBO(mVBOMatrices, mMatrices);
	}
}

void MeshBuffer::setIndexesData(const Mesh& mesh)
{
	RenderContext::resizeEBO(mEBO, mesh.getFaces().size(), mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	RenderContext::setDataEBO(mEBO, mesh.getFaces());
}

void MeshBuffer::addInstanceMatrix(const Matrix4& modelMatrix)
{
	if(mIsInstanced)
	{
		std::copy(modelMatrix.getData(), modelMatrix.getData() + Matrix4::smMatrixSize, back_inserter(mMatrices));
	}
}

void MeshBuffer::clear()
{
	if(mIsInstanced)
	{
		mMatrices.clear();
	}
}

void MeshBuffer::setMaxInstances(u32 maxInstances)
{
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

// ------------------------------------------------------
// ------------------------------------------------------

MeshBatcher::~MeshBatcher()
{
}

void MeshBatcher::init(Ref<const Mesh> prototypeMesh, bool isStatic, bool isInstanced)
{
	terminate();
	mMeshBuffer.init(isStatic, isInstanced);
	mPrototypeMesh = prototypeMesh;
	
}

void MeshBatcher::clear()
{
	mMeshBuilder.clear();
	mMeshBuffer.clear();
}

void MeshBatcher::resize(u32 size)
{
	clear();
	
	if (size > mMaxMeshesThreshold)
	{
		mMaxMeshesThreshold += mMaxMeshesIncrement;

		u32 meshesAmount = mMeshBuffer.getIsInstanced() ? 1 : mMaxMeshesThreshold; 

		mMeshBuilder.init(mPrototypeMesh.get().getVertexCount() * meshesAmount, mPrototypeMesh.get().getFacesCount() * meshesAmount);

		mMeshBuffer.setMaxInstances(meshesAmount);

		generateFacesData(meshesAmount);
	}

	if(mMeshBuffer.getIsInstanced())
	{
		mMeshBuilder.copyVertices(mPrototypeMesh);
		mMeshBuilder.copyTextureCoordinates(mPrototypeMesh);
		//mMeshBuilder.copyColors(mMesh);

		FOR_RANGE(i, 0, mPrototypeMesh.get().getVertexCount())
		{
			mMeshBuilder.addColor(0,0,0,1);
		}
	}

	mMeshBuffer.resize(mMeshBuilder);

	mMeshesIndex = 0;

	mDataSentToGPU = false;
}

void MeshBatcher::generateFacesData(u32 meshesCount)
{
	// Create Faces once and send to GPU once.
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

void MeshBatcher::addInstanceMatrix(const Matrix4& modelMatrix)
{
	mMeshBuffer.addInstanceMatrix(modelMatrix);

	mMeshesIndex++;
}

void MeshBatcher::addInstance(const Mesh& meshInstance)
{
	mMeshBuilder.addVertices(meshInstance.getVertices());
	mMeshBuilder.addTextureCoordinates(meshInstance.getTextureCoordinates());
	mMeshBuilder.addColors(meshInstance.getColors());

	mMeshesIndex++;
}

void MeshBatcher::sendDataToGPU()
{
	if(!mDataSentToGPU)
	{
		mMeshBuffer.setData(mMeshBuilder);
		mDataSentToGPU = true;
	}
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