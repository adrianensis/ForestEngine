#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"

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