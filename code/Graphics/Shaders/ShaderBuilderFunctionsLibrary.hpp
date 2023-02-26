#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shaders/ShaderBuilderNodes.hpp"

class ShaderBuilderFunctionsLibrary
{
public:
    ShaderBuilderNodes::FunctionDefinition getFunctionCalculateSkinnedPosition(const ShaderBuilderNodes::Program& program);
public:
    std::unordered_map<std::string_view, ShaderBuilderNodes::FunctionDefinition> mFunctions;
};