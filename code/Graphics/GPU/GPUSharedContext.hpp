#pragma once

#include "Graphics/GPU/GPUUniformBuffer.hpp"

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
    u32 requestUniformBufferBindingPoint();
    
public:
	GPUUniformBuffer mGlobalMatricesBlock;
	GPUUniformBuffer mLightsBlock;

private:
    u32 mBindingPointsIndex = 0;
    i32 mMaxUniformBufferBindingPoints = 0;
};