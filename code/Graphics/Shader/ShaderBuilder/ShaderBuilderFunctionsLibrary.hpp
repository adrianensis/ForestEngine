#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"

class Material;

class ShaderBuilderFunctionsLibrary
{
public:
    void init(const ShaderBuilderNodes::Program& program, Ptr<const Material> material);
private:
    void registerFunctionCalculateSkinnedPosition(const ShaderBuilderNodes::Program& program, Ptr<const Material> material);
    void registerFunctionCalculateDiffuse(const ShaderBuilderNodes::Program& program, Ptr<const Material> material);
public:
    std::unordered_map<std::string, ShaderBuilderNodes::FunctionDefinition> mFunctions;
};