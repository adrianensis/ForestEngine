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
	// Force static draw if instanced
	mVBOPosition.init(Mesh::smVertexPositionSize, 0, mIsStatic || mIsInstanced);
	mVBOTexture.init(Mesh::smVertexTexCoordSize, 1, mIsStatic || mIsInstanced);
	mVBOColor.init(Mesh::smVertexColorSize, 2, mIsStatic || mIsInstanced);

	if(mIsInstanced)
	{
		glGenBuffers(1, &mVBOMatrices);
		glBindBuffer(GL_ARRAY_BUFFER, mVBOMatrices);

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

	mVBOBones.init(7, mIsStatic || mIsInstanced);

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

void MeshBuffer::resize(const Mesh& mesh)
{
	mVBOPosition.resize(mesh.getVertices().capacity());
	mVBOTexture.resize(mesh.getTextureCoordinates().capacity());
	mVBOColor.resize(mesh.getColors().capacity());
	mVBOBones.resize(mesh.getBonesVertexData().capacity());
	
	if(mIsInstanced)
	{
		RenderContext::resizeVBOAnyType(mVBOMatrices, sizeof(Matrix4), mMatrices.capacity(), mIsStatic || mIsInstanced ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}
}

void MeshBuffer::setData(const Mesh& mesh)
{
	mVBOPosition.setData(mesh.getVertices());
	mVBOTexture.setData(mesh.getTextureCoordinates());
	mVBOColor.setData(mesh.getColors());
	mVBOBones.setData(mesh.getBonesVertexData());
}

void MeshBuffer::setIndexesData(const Mesh& mesh)
{
	RenderContext::resizeEBO(mEBO, mesh.getFaces().size(), mIsStatic || mIsInstanced ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	RenderContext::setDataEBO(mEBO, mesh.getFaces());
}

void MeshBuffer::addInstanceMatrix(const Matrix4& modelMatrix)
{
	PROFILER_CPU()

	if(mIsInstanced)
	{
		mMatrices.push_back(modelMatrix);
		//std::copy(modelMatrix.getData(), modelMatrix, back_inserter(mMatrices));
	}
}

void MeshBuffer::setDataInstanced()
{
	RenderContext::setDataVBOAnyType<Matrix4>(mVBOMatrices, mMatrices);
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
		RenderContext::resizeVBOAnyType(mVBOMatrices, sizeof(Matrix4), mMatrices.capacity(), mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
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

		mMeshBuilder.addVertices(mPrototypeMesh.get().getVertices());
		mMeshBuilder.addTextureCoordinates(mPrototypeMesh.get().getTextureCoordinates());
		FOR_RANGE(i, 0, mPrototypeMesh.get().getVertexCount())
		{
			mMeshBuilder.addColor(0,0,0,1);
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
			mMeshBuilder.addFaceIndex(mPrototypeMesh.get().getFaces()[faceIndex] + offset);
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
