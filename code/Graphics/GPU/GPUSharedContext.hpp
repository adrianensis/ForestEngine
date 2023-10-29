#pragma once

#include "Graphics/GPU/GPUBlock.hpp"


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
    
public:
	GPUBlock mMatricesBlock;
};