#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUInstance.hpp"

GPUProgram::GPUProgram()
{
};

void GPUProgram::enable() const
{
//	GET_SYSTEM(GPUInterface).enableProgram(mProgramId);
}

void GPUProgram::disable() const
{
//	GET_SYSTEM(GPUInterface).disableProgram(mProgramId);
}

void GPUProgram::bindUniformBuffer(const GPUUniformBuffer& uniformBuffer)
{
//    GET_SYSTEM(GPUInterface).bindUniformBufferToShader(mProgramId,
    // uniformBuffer.getGPUUniformBufferData().mType,
    // uniformBuffer.getGPUUniformBufferData().mBufferName,
    // uniformBuffer.getBindingPoint());

    // mUniformBuffers.push_back(uniformBuffer);
}

void GPUProgram::initFromFileContents(Ptr<GPUContext> gpuContext, const std::string& vertex, const std::string& fragment)
{
//    mProgramId = GET_SYSTEM(GPUInterface).compileProgram(vertex, fragment);
    mGPUContext = gpuContext;
}

void GPUProgram::initFromFilePaths(Ptr<GPUContext> gpuContext, const std::string& vertex, const std::string& fragment)
{
	LOG_TRACE()
    mGPUContext = gpuContext;

    std::ifstream vertex_ifs;
	vertex_ifs.open(vertex.c_str(), std::ifstream::in);
	std::string vertexShaderSource((std::istreambuf_iterator<char>(vertex_ifs)), std::istreambuf_iterator<char>());

    std::ifstream fragment_ifs;
	fragment_ifs.open(fragment.c_str(), std::ifstream::in);
	std::string fragmentShaderSource((std::istreambuf_iterator<char>(fragment_ifs)), std::istreambuf_iterator<char>());

    initFromFileContents(mGPUContext, vertexShaderSource, fragmentShaderSource);
}

void GPUProgram::terminate()
{

}