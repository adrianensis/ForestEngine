#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "GPU/Buffer/GPUVertexBuffer.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/Buffer/GPUUniformBuffer.hpp"

class ShaderBuilder
{
public:
    ShaderBuilder();
    ShaderBuilderNodes::Program& get() { return mProgram; }
    ShaderBuilderNodes::BlockStatement& getMain();
    std::string getCode() const;

    void setVariableInCache(ShaderBuilderNodes::Variable& variable);
    void removeVariableFromCache(ShaderBuilderNodes::Variable& variable);
    const ShaderBuilderNodes::Variable& getVariableFromCache(const std::string& variableName) const;

private:
    ShaderBuilderNodes::Program mProgram;
    std::unordered_map<std::string, ShaderBuilderNodes::Variable> mVariablesCache;
};