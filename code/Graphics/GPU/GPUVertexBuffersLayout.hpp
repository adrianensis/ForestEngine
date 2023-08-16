#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"

class GPUVertexBuffersLayout
{
public:
	GPUVertexBuffersLayout() = default;
    ~GPUVertexBuffersLayout();

    void init(bool isStatic);
    u32 addBuffer(const GPUVertexBufferData& data);
    GPUVertexBuffer& getBuffer(u32 index);

private:
    void terminate();

private:
    std::vector<GPUVertexBuffer> mBuffers;
    u32 mAttributeIndex = 0;
	bool mIsStatic = false;
public:
    CRGET(Buffers)
};
