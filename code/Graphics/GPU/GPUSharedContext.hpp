#pragma once

#include "Graphics/GPU/GPUSharedBuffer.hpp"

class GPUSharedContextMatricesData
{
public:
	Matrix4 mProjectionMatrix;
	Matrix4 mViewMatrix;
};

class GPUSharedContext : public System
{
public:
	GPUSharedContext() = default;

    void init();
    void terminate();
    u32 requestSharedBufferBindingPoint(GPUSharedBufferType gpuSharedBufferType);
    
public:
	GPUSharedBuffer mGlobalMatricesBlock;
	GPUSharedBuffer mLightsBlock;

private:
    u32 mBindingPointsIndexUniform = 0;
    u32 mBindingPointsIndexStorage = 0;
    i32 mMaxSharedBufferBindingPointsUniform = 0;
    i32 mMaxSharedBufferBindingPointsStorage = 0;
};