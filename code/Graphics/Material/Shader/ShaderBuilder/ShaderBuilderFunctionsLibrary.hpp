#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"

class Material;

class ShaderBuilderFunctionsLibrary
{
public:
    void init(const ShaderBuilderNodes::Program& program, Ptr<const Material> material);
private:
    void registerFunctionCalculateBoneTransform(const ShaderBuilderNodes::Program& program, Ptr<const Material> material);
    void registerFunctionCalculatePhong(const ShaderBuilderNodes::Program& program, Ptr<const Material> material);
public:
    std::unordered_map<std::string, ShaderBuilderNodes::FunctionDefinition> mFunctions;
};