#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

class GPUBlockData
{
public:
	GPUBlockData() = default;
	GPUBlockData(const std::vector<GPUVariableData>& gpuVariableDataArray, const std::string& blockName): mGPUVariableDataArray(gpuVariableDataArray), mBlockName(blockName) {}
	GPUBlockData(const std::vector<GPUVariableData>& gpuVariableDataArray, const std::string& blockName, const std::string& instanceName): mGPUVariableDataArray(gpuVariableDataArray), mBlockName(blockName), mInstanceName(instanceName) {}

public:
    std::vector<GPUVariableData> mGPUVariableDataArray;
    std::string mBlockName;
    std::string mInstanceName;
};

class GPUBlock
{
public:
	GPUBlock() = default;

    void init(const GPUBlockData& gpuBlockData, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setData(const T& data)
    {
	    GET_SYSTEM(GPUInterface).setDataUBOAnyStruct(mUBO, data);
    }
    void terminate();

protected:
    GPUBlockData mGPUBlockData;
	u32 mUBO = 0;
    bool mIsStatic = false;
    u32 mSizeInBytes = 0;
public:
    GET(SizeInBytes)
    CRGET(GPUBlockData)
};