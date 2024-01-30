#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUBuffersLayout.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"

class ShaderBuilder
{
public:
    ShaderBuilder() = default;
    ShaderBuilderNodes::Program& get() { return mProgram; }
    std::string getCode() const;

    void setVariableInCache(ShaderBuilderNodes::Variable& variable);
    void removeVariableFromCache(ShaderBuilderNodes::Variable& variable);
    const ShaderBuilderNodes::Variable& getVariableFromCache(const std::string& variableName) const;

private:
    ShaderBuilderNodes::Program mProgram;
    std::unordered_map<std::string, ShaderBuilderNodes::Variable> mVariablesCache;
};