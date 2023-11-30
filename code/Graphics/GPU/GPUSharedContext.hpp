#pragma once

#include "Graphics/GPU/GPUUniformBlock.hpp"

class GPUSharedContextMatricesData
{
public:
	Matrix4 mProjectionMatrix;
	Matrix4 mViewMatrix;
};

class GPUSharedContext : public EngineSystem
{
public:
	GPUSharedContext() = default;

    void init();
    void terminate();
    u32 requestUniformBufferBindingPoint();
    
public:
	GPUUniformBlock mGlobalMatricesBlock;
	GPUUniformBlock mLightsBlock;

private:
    u32 mBindingPointsIndex = 0;
    i32 mMaxUniformBufferBindingPoints = 0;
};