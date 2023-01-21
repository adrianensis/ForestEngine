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
	
	if(mMeshBuffer.mIsInstanced)
	{
		mMeshBuilder.init(mPrototypeMesh.get().mVertexCount * 1, mPrototypeMesh.get().mFacesCount * 1);

		mMeshBuilder.appendToPositions(mPrototypeMesh.get().mPositions);
		mMeshBuilder.appendToTextureCoordinates(mPrototypeMesh.get().mTextureCoordinates);
		FOR_RANGE(i, 0, mPrototypeMesh.get().mVertexCount)
		{
			mMeshBuilder.addToColors(Vector4(0,0,0,1));
		}
		mMeshBuilder.appendToBonesVertexData(mPrototypeMesh.get().mBonesVertexData);

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

		if (mMeshBuffer.mIsInstanced)
		{
			mMeshBuffer.setMaxInstances(mMaxMeshesThreshold);
		}
		else
		{
			mMeshBuilder.init(mPrototypeMesh.get().mVertexCount * mMaxMeshesThreshold, mPrototypeMesh.get().mFacesCount * mMaxMeshesThreshold);
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

	mMeshBuilder.appendToPositions(meshInstance.mPositions);
	mMeshBuilder.appendToTextureCoordinates(meshInstance.mTextureCoordinates);
	mMeshBuilder.appendToColors(meshInstance.mColors);
	mMeshBuilder.appendToBonesVertexData(meshInstance.mBonesVertexData);

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
		RenderContext::drawElements(mPrototypeMesh.get().mFaces.size() * 3, mMeshesIndex, mMeshBuffer.mIsInstanced);
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
	if(mPrototypeMesh.get().mModel)
	{
		isAnimated = mPrototypeMesh.get().mModel.get().isAnimated();
	}

	return isAnimated;
}

void MeshBatcher::clear()
{
	PROFILER_CPU()
	if( ! mMeshBuffer.mIsInstanced)
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
		u32 offset = (i * mPrototypeMesh.get().mVertexCount);
		
		FOR_RANGE(faceIndex, 0, mPrototypeMesh.get().mFaces.size())
		{
			Face newFace = mPrototypeMesh.get().mFaces[faceIndex];
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
    if(mMeshBuffer.mIsInstanced)
    {
        mMeshBuffer.setDataInstanced();
    }
    else
    {
        mMeshBuffer.setData(mMeshBuilder);
    }

    mDataSentToGPU = true;
}
