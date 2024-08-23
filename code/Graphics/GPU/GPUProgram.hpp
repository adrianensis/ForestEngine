#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUContext.hpp"
#include "Graphics/GPU/Vulkan.h"
#include "Core/Object/ObjectBase.hpp"

class GPUVertexBuffersContainer;
class GPUUniformBuffer;

class GPUProgram: public ObjectBase
{    
public:

    GPUProgram();
	~GPUProgram() override = default;

    void initFromFilePaths(Ptr<GPUContext> gpuContext, const std::string& vertex, const std::string& fragment);
    void initFromFileContents(Ptr<GPUContext> gpuContext, const std::string& vertex, const std::string& fragment);
    void terminate();
    void enable() const;
    void disable() const;
    template<class T>
    void bindUniformValue(const HashedString& name, const T& value)
    {
//        GET_SYSTEM(GPUInterface).bindUniformValue<T>(mProgramId, name, value);
    }
    void bindUniformBuffer(const GPUUniformBuffer& uniformBuffer);

private:
	u32 mProgramId = 0;

    Ptr<GPUContext> mGPUContext;
};
REGISTER_CLASS(GPUProgram);
