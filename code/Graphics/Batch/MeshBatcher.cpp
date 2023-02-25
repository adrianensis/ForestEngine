#include "Graphics/Batch/MeshBatcher.hpp"
#include "Graphics/Model/Model.hpp"

void MeshBatcher::init(Ptr<const Mesh> prototypeMesh, bool isStatic, bool isInstanced)
{
	PROFILER_CPU()

	mGPUMeshBuffer.init(isStatic, isInstanced);
	mPrototypeMesh = prototypeMesh;
	
	if(mGPUMeshBuffer.mIsInstanced)
	{
		mMeshBuilder.init(mPrototypeMesh.get().mVertexCount * 1, mPrototypeMesh.get().mFacesCount * 1);

		mMeshBuilder.appendToPositions(mPrototypeMesh.get().mPositions);
		mMeshBuilder.appendToTextureCoordinates(mPrototypeMesh.get().mTextureCoordinates);
        mMeshBuilder.appendToColors(mPrototypeMesh.get().mColors);
		mMeshBuilder.appendToBonesVertexIDsData(mPrototypeMesh.get().mBonesVertexIDsData);
		mMeshBuilder.appendToBonesVertexWeightsData(mPrototypeMesh.get().mBonesVertexWeightsData);

		generateFacesData(1);
		mGPUMeshBuffer.resize(mMeshBuilder);

		mDataSentToGPU = false;

		mGPUMeshBuffer.setData(mMeshBuilder);

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

		if (mGPUMeshBuffer.mIsInstanced)
		{
			mGPUMeshBuffer.setMaxInstances(mMaxMeshesThreshold);
		}
		else
		{
			mMeshBuilder.init(mPrototypeMesh.get().mVertexCount * mMaxMeshesThreshold, mPrototypeMesh.get().mFacesCount * mMaxMeshesThreshold);
			generateFacesData(mMaxMeshesThreshold);
			mGPUMeshBuffer.resize(mMeshBuilder);
		}
	}

	mMeshesIndex = 0;

	mDataSentToGPU = false;
}

void MeshBatcher::addInstanceMatrix(const Matrix4& modelMatrix)
{
	PROFILER_CPU()

	mGPUMeshBuffer.addInstanceMatrix(modelMatrix);

	mMeshesIndex++;
}

void MeshBatcher::addInstance(Ptr<const Mesh> meshInstance)
{
	PROFILER_CPU()

	mMeshBuilder.appendToPositions(meshInstance.get().mPositions);
	mMeshBuilder.appendToTextureCoordinates(meshInstance.get().mTextureCoordinates);
	mMeshBuilder.appendToColors(meshInstance.get().mColors);
	mMeshBuilder.appendToBonesVertexIDsData(meshInstance.get().mBonesVertexIDsData);
	mMeshBuilder.appendToBonesVertexWeightsData(meshInstance.get().mBonesVertexWeightsData);

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
		GET_SYSTEM(RenderContext).drawElements(mPrototypeMesh.get().mFaces.size() * 3, mMeshesIndex, mGPUMeshBuffer.mIsInstanced);
	}
}

void MeshBatcher::enable()
{
	mGPUMeshBuffer.enable();
}

void MeshBatcher::disable()
{
	mGPUMeshBuffer.disable();
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
	if( ! mGPUMeshBuffer.mIsInstanced)
	{
		mMeshBuilder.clear();
	}
	mGPUMeshBuffer.clear();
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

	mGPUMeshBuffer.setIndexesData(mMeshBuilder);
}

void MeshBatcher::sendDataToGPU()
{	
    if(mGPUMeshBuffer.mIsInstanced)
    {
        mGPUMeshBuffer.setDataInstanced();
    }
    else
    {
        mGPUMeshBuffer.setData(mMeshBuilder);
    }

    mDataSentToGPU = true;
}
