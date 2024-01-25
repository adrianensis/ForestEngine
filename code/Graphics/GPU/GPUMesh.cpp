#include "Graphics/GPU/GPUMesh.hpp"

void GPUMesh::init(u32 vertexCount, u32 indicesCount, const std::vector<GPUVariableData>& gpuVertexInputBuffers)
{
	mVertexCount = vertexCount;
	mIndicesCount = indicesCount;

    mGPUVertexInputBuffers = gpuVertexInputBuffers;

    mBuffers.clear();
    FOR_ARRAY(i, mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mGPUVertexInputBuffers[i];
        mBuffers.emplace(gpuVariableData.mName, gpuVariableData.mGPUDataType.mTypeSizeInBytes);
    }

	clear();
}

void GPUMesh::clear()
{
    FOR_MAP(it, mBuffers)
    {
        it->second.clear();
        it->second.reserve(mVertexCount);
    }

	mIndices.clear();
	mIndices.reserve(mIndicesCount);
}

void GPUMesh::setColor(const Vector4 &color)
{
    mBuffers.at(GPUBuiltIn::VertexInput::mColor.mName).clear();
    mBuffers.at(GPUBuiltIn::VertexInput::mColor.mName).resize(mVertexCount);
    mBuffers.at(GPUBuiltIn::VertexInput::mColor.mName).fill(color);
}
