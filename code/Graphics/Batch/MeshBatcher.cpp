#include "Graphics/Batch/MeshBatcher.hpp"
#include "Graphics/Model/Model.hpp"

MeshBatcher::~MeshBatcher() 
{
}

void MeshBatcher::init(Ptr<const Mesh> prototypeMesh, bool isStatic, bool isInstanced)
{
	PROFILER_CPU()

	mMeshBuffer.init(isStatic, isInstanced);
	mPrototypeMesh = prototypeMesh;
	
	if(mMeshBuffer.getIsInstanced())
	{
		mMeshBuilder.init(mPrototypeMesh.get().getVertexCount() * 1, mPrototypeMesh.get().getFacesCount() * 1);

		mMeshBuilder.appendToPositions(mPrototypeMesh.get().getPositions());
		mMeshBuilder.appendToTextureCoordinates(mPrototypeMesh.get().getTextureCoordinates());
		FOR_RANGE(i, 0, mPrototypeMesh.get().getVertexCount())
		{
			mMeshBuilder.addToColors(Vector4(0,0,0,1));
		}
		mMeshBuilder.appendToBonesVertexData(mPrototypeMesh.get().getBonesVertexData());

		generateFacesData(1);
		mMeshBuffer.resize(mMeshBuilder);

		mDataSentToGPU = false;

		mMeshBuffer.setData(mMeshBuilder);

		mMeshesIndex = 0;
	}

    disable();
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

	mMeshBuilder.appendToPositions(meshInstance.getPositions());
	mMeshBuilder.appendToTextureCoordinates(meshInstance.getTextureCoordinates());
	mMeshBuilder.appendToColors(meshInstance.getColors());
	mMeshBuilder.appendToBonesVertexData(meshInstance.getBonesVertexData());

	mMeshesIndex++;
}

void MeshBatcher::drawCall()
{	
	if (mMeshesIndex > 0)
	{
        if(!mDataSentToGPU)
        {
		    sendDataToGPU();
        }
		RenderContext::drawElements(mPrototypeMesh.get().getFaces().size() * 3, mMeshesIndex, mMeshBuffer.getIsInstanced());
	}
}

void MeshBatcher::enable()
{
	mMeshBuffer.enable();
}

void MeshBatcher::disable()
{
	mMeshBuffer.disable();
}

bool MeshBatcher::isAnimated() const
{
	bool isAnimated = false;
	if(mPrototypeMesh.get().getModel())
	{
		isAnimated = mPrototypeMesh.get().getModel().get().isAnimated();
	}

	return isAnimated;
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

			mMeshBuilder.addToFaces(newFace);
		}
	}

	mMeshBuffer.setIndexesData(mMeshBuilder);
}

void MeshBatcher::sendDataToGPU()
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
