#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"

class Material;

class ShaderBuilderFunctionsLibrary
{
public:
    ShaderBuilderNodes::FunctionDefinition getFunctionCalculateSkinnedPosition(const ShaderBuilderNodes::Program& program, Ptr<const Material> material);
public:
    std::unordered_map<std::string_view, ShaderBuilderNodes::FunctionDefinition> mFunctions;
};