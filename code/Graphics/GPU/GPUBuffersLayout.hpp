#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUBuffer.hpp"

class GPUBuffersLayout
{
public:
	GPUBuffersLayout() = default;
    ~GPUBuffersLayout();

    void init(bool isStatic);
    u32 addBuffer(const GPUBufferData& data);
    GPUBuffer& getBuffer(u32 index);

private:
    void terminate();

private:
    std::vector<GPUBuffer> mBuffers;
    u32 mAttributeIndex = 0;
	bool mIsStatic = false;
public:
    CRGET(Buffers)
};
