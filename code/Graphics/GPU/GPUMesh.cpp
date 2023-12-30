#include "Graphics/GPU/GPUMesh.hpp"

void GPUMesh::init(u32 vertexCount, u32 facesCount, const GPUVertexInputBuffers gpuVertexInputBuffers)
{
	mVertexCount = vertexCount;
	mFacesCount = facesCount;

    mGPUVertexInputBuffers = gpuVertexInputBuffers;

    mGPUMeshByteBuffers.mBuffers.clear();
    FOR_ARRAY(i, gpuVertexInputBuffers.mBuffers)
    {
        const GPUVariableData& gpuVariableData = gpuVertexInputBuffers.mBuffers[i];
        mGPUMeshByteBuffers.mBuffers.emplace(gpuVariableData.mName, gpuVariableData.mGPUDataType.mTypeSizeInBytes);
    }

	clear();
}

void GPUMesh::clear()
{
    FOR_MAP(it, mGPUMeshByteBuffers.mBuffers)
    {
        it->second.clear();
        it->second.reserve(mVertexCount);
    }

	mFaces.clear();
	mFaces.reserve(mFacesCount);
}

void GPUMesh::setColor(const Vector4 &color)
{
    mGPUMeshByteBuffers.mBuffers.at(GPUBuiltIn::VertexInput::mColor.mName).clear();
    mGPUMeshByteBuffers.mBuffers.at(GPUBuiltIn::VertexInput::mColor.mName).resize(mVertexCount);
    mGPUMeshByteBuffers.mBuffers.at(GPUBuiltIn::VertexInput::mColor.mName).fill(color);
}
