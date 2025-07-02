#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"

void GPUMesh::init(Core::u32 vertexCount, Core::u32 indicesCount, const std::vector<GPUVariableData>& gpuVertexInputBuffers)
{
    if (mMeshID == 0)
    {
        mMeshID = smMeshIdCounter++;
    }

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

void GPUMesh::setColor(const Maths::Vector4 &color)
{
    mBuffers.at(GPUShaderDefinitions::VertexInput::mColor.mName).clear();
    mBuffers.at(GPUShaderDefinitions::VertexInput::mColor.mName).resize(mVertexCount);
    mBuffers.at(GPUShaderDefinitions::VertexInput::mColor.mName).fill(color);
}

bool GPUMesh::hasVertexInputBuffer(const GPUVariableData& data) const
{
    return mBuffers.contains(data.mName);
}