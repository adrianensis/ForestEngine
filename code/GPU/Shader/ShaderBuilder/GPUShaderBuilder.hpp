#pragma once

#include "Core/Minimal.hpp"
#include "GPU/Shader/ShaderBuilder/GPUShaderBuilderNodes.hpp"
#include "GPU/Buffer/GPUVertexBuffer.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/Buffer/GPUUniformBuffer.hpp"

class GPUShaderBuilder
{
public:
    GPUShaderBuilder();
    ~GPUShaderBuilder();
    GPUShaderBuilderNodes::Program& get() { return mProgram; }
    GPUShaderBuilderNodes::BlockStatement& getMain();
    std::string getCode() const;

    void setVariableInCache(GPUShaderBuilderNodes::Variable& variable);
    void removeVariableFromCache(GPUShaderBuilderNodes::Variable& variable);
    const GPUShaderBuilderNodes::Variable& getVariableFromCache(const std::string& variableName) const;

private:
    GPUShaderBuilderNodes::Program mProgram;
    std::unordered_map<std::string, GPUShaderBuilderNodes::Variable> mVariablesCache;
};