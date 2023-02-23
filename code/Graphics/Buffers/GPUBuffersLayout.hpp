#pragma once

#include "Core/Module.hpp"
#include "Graphics/Buffers/GPUBuffer.hpp"

class GPUBuffersLayout
{
public:
	GPUBuffersLayout() = default;
	
    ~GPUBuffersLayout();

    void init(bool isStatic);
    u32 addBuffer(u32 typeSizeInBytes);
    GPUBuffer& getBuffer(u32 index);

private:
    void terminate();

private:
    std::vector<GPUBuffer> mBuffers;
    u32 mAttributeIndex = 0;
public:
	bool mIsStatic = false;
};
