#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Buffers/VertexBuffer.hpp"
#endif

// A single buffer of data
class VertexBuffer: public ObjectBase
{
    GENERATE_METADATA(VertexBuffer)

public:
	VertexBuffer() = default;
	CPP ~VertexBuffer() override
	{
		//terminate();
	}

	CPP void init(u32 elementSize, u32 propertyArrayIndex, bool isStatic)
	{
		terminate();

		mIsStatic = isStatic;
		mVBO = RenderContext::createVBO(elementSize, propertyArrayIndex);
		mGenerated = true;
	}

	CPP void terminate()
	{
		if(mGenerated)
		{
			glDeleteBuffers(1, &mVBO);
			mGenerated = false;
		}
	}

	CPP void resize(u32 size)
	{
		RenderContext::resizeVBO(mVBO, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	CPP void setData(const std::vector<f32> &data)
	{
		RenderContext::setDataVBO(mVBO, data);
	}

private:
	bool mGenerated = false;

	bool mIsStatic = false;
	u32 mVBO = 0; // TODO: change u32 for GLuint
	u32 mAttribPointerIndex = 0;
};

// Buffers data for a mesh
class MeshBuffer: public ObjectBase
{
    GENERATE_METADATA(MeshBuffer)

public:
	MeshBuffer() = default;
	
	CPP ~MeshBuffer() override
	{
		//terminate();
	}

	CPP void init(bool isStatic, bool isInstanced)
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

	CPP void terminate()
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

	CPP void resize(const Mesh& mesh)
	{
		mVBOPosition.resize(mesh.getVertices().capacity());
		mVBOTexture.resize(mesh.getTextureCoordinates().capacity());
		mVBOColor.resize(mesh.getColors().capacity());

		if(mIsInstanced)
		{
			RenderContext::resizeVBO(mVBOMatrices, mMatrices.capacity(), mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
		}
	}

	CPP void setData(const Mesh& mesh)
	{
		mVBOPosition.setData(mesh.getVertices());
		mVBOTexture.setData(mesh.getTextureCoordinates());
		mVBOColor.setData(mesh.getColors());

		if(mIsInstanced)
		{
			RenderContext::setDataVBO(mVBOMatrices, mMatrices);
		}
	}

	CPP void setIndexesData(const Mesh& mesh)
	{
		RenderContext::resizeEBO(mEBO, mesh.getFaces().size(), mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
		RenderContext::setDataEBO(mEBO, mesh.getFaces());
	}

	CPP void addInstanceMatrix(const Matrix4& modelMatrix)
	{
		if(mIsInstanced)
		{
			std::copy(modelMatrix.getData(), modelMatrix.getData() + Matrix4::smMatrixSize, back_inserter(mMatrices));
		}
	}

	CPP void clear()
	{
		if(mIsInstanced)
		{
			mMatrices.clear();
		}
	}

	CPP void setMaxInstances(u32 maxInstances)
	{
		if(mIsInstanced)
		{
			mMatrices.reserve(Matrix4::smMatrixSize * maxInstances);
		}
	}

	CPP void enable()
	{
		RenderContext::enableVAO(mVAO);
	}

	CPP void disable()
	{
		RenderContext::enableVAO(0);
	}


private:
	bool mGenerated = false;

	bool mIsStatic = false;

	u32 mVBOMatrices = 0;
	bool mIsInstanced = false;
	std::vector<f32> mMatrices;
	
	u32 mVAO = 0;
	VertexBuffer mVBOPosition;
	VertexBuffer mVBOTexture;
	VertexBuffer mVBOColor;
	u32 mEBO = 0;
public:
	GET(IsInstanced)
};

// Combines all the data of multiple instances of the same Mesh
class MeshBatcher: public ObjectBase
{
    GENERATE_METADATA(MeshBatcher)

public:
	MeshBatcher() = default;

	CPP ~MeshBatcher() override
	{
	}

	CPP void init(Ptr<const Mesh> prototypeMesh, bool isStatic, bool isInstanced)
	{
		terminate();
		mMeshBuffer.init(isStatic, isInstanced);
		mPrototypeMesh = prototypeMesh;
		
	}

	CPP void resize(u32 size)
	{
		clear();

		if (size > mMaxMeshesThreshold)
		{
			// TODO: find a better way
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

	CPP void addInstanceMatrix(const Matrix4& modelMatrix)
	{
		mMeshBuffer.addInstanceMatrix(modelMatrix);

		mMeshesIndex++;
	}

	CPP void addInstance(const Mesh& meshInstance)
	{
		mMeshBuilder.addVertices(meshInstance.getVertices());
		mMeshBuilder.addTextureCoordinates(meshInstance.getTextureCoordinates());
		mMeshBuilder.addColors(meshInstance.getColors());

		mMeshesIndex++;
	}

	CPP void drawCall()
	{
		if (mMeshesIndex > 0)
		{
			sendDataToGPU();
			RenderContext::drawElements(mPrototypeMesh.get().getFaces().size(), mMeshesIndex, mMeshBuffer.getIsInstanced());
		}
	}

	CPP void terminate()
	{
		mMeshBuffer.terminate();
	}

	CPP void enable()
	{
		mMeshBuffer.enable();
	}

	CPP void disable()
	{
		mMeshBuffer.disable();
	}

private:
	CPP void clear()
	{
		mMeshBuilder.clear();
		mMeshBuffer.clear();
	}

	CPP void generateFacesData(u32 meshesCount)
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

	CPP void sendDataToGPU()
	{
		if(!mDataSentToGPU)
		{
			mMeshBuffer.setData(mMeshBuilder);
			mDataSentToGPU = true;
		}
	}
	
private:
	Ptr<const Mesh> mPrototypeMesh;
	Mesh mMeshBuilder;
	MeshBuffer mMeshBuffer;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;

	u32 mMeshesIndex = 0;

	bool mDataSentToGPU = false;

public:
	RGET(MeshBuilder)
};