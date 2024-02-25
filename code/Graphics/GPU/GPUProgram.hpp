#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

class GPUVertexBuffersContainer;
class GPUSharedBuffer;

class GPUProgram: public ObjectBase
{
    
public:

    GPUProgram();
	~GPUProgram() override = default;

    void initFromFilePaths(const std::string& vertex, const std::string& fragment);
    void initFromFileContents(const std::string& vertex, const std::string& fragment);
    void enable() const;
    void disable() const;
    template<class T>
    void bindUniformValue(const std::string& name, const T& value)
    {
        GET_SYSTEM(GPUInterface).bindUniformValue<T>(mProgramId, name, value);
    }
    void bindSharedBuffer(const GPUSharedBuffer& sharedBuffer);

private:
	u32 mProgramId = 0;
};
REGISTER_CLASS(GPUProgram);
