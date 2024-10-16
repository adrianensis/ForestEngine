#include "Graphics/GPU/Mesh/GPUMesh.hpp"

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

bool GPUMesh::hasVertexInputBuffer(const GPUVariableData& data) const
{
    return mBuffers.contains(data.mName);
}

void GPUMesh::populateGPUVertexBuffersContainer(GPUVertexBuffersContainer& gpuVertexBuffersContainer, bool isStatic) const
{
    FOR_ARRAY(i, mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mGPUVertexInputBuffers[i];
        GPUVertexBufferData bufferData(gpuVariableData);
        gpuVertexBuffersContainer.addVertexBuffer(bufferData, isStatic);
    }

    GPUVertexBufferData bufferDataInstanceIDs(GPUBuiltIn::VertexInput::mInstanceID, 1);
    gpuVertexBuffersContainer.addVertexBuffer(bufferDataInstanceIDs, isStatic);

    GPUVertexBufferData bufferDataObjectIDs(GPUBuiltIn::VertexInput::mObjectID, 1);
    gpuVertexBuffersContainer.addVertexBuffer(bufferDataObjectIDs, isStatic);

    GPUVertexBufferData bufferDataMaterialInstanceIDs(GPUBuiltIn::VertexInput::mMaterialInstanceID, 1);
    gpuVertexBuffersContainer.addVertexBuffer(bufferDataMaterialInstanceIDs, isStatic);
}
