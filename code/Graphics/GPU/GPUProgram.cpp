#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUBuffersLayout.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"

GPUProgram::GPUProgram()
{
};

void GPUProgram::enable() const
{
	GET_SYSTEM(GPUInterface).enableProgram(mProgramId);
}

void GPUProgram::disable() const
{
	GET_SYSTEM(GPUInterface).disableProgram(mProgramId);
}

void GPUProgram::bindSharedBuffer(const GPUSharedBuffer& sharedBuffer)
{
    GET_SYSTEM(GPUInterface).bindSharedBufferToShader(mProgramId,
    sharedBuffer.getGPUSharedBufferData().mType,
    sharedBuffer.getGPUSharedBufferData().mBufferName,
    sharedBuffer.getBindingPoint());
}

void GPUProgram::initFromFileContents(const std::string& vertex, const std::string& fragment)
{
    mProgramId = GET_SYSTEM(GPUInterface).compileProgram(vertex, fragment);
}

void GPUProgram::initFromFilePaths(const std::string& vertex, const std::string& fragment)
{
	LOG_TRACE()

    std::ifstream vertex_ifs;
	vertex_ifs.open(vertex.c_str(), std::ifstream::in);
	std::string vertexShaderSource((std::istreambuf_iterator<char>(vertex_ifs)), std::istreambuf_iterator<char>());

    std::ifstream fragment_ifs;
	fragment_ifs.open(fragment.c_str(), std::ifstream::in);
	std::string fragmentShaderSource((std::istreambuf_iterator<char>(fragment_ifs)), std::istreambuf_iterator<char>());

    initFromFileContents(vertexShaderSource, fragmentShaderSource);
}
