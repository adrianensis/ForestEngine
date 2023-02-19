#pragma once

#include "Core/Module.hpp"
#include "Graphics/Shaders/ShaderBuilderNodes.hpp"

class ShaderBuilder
{
public:
    ShaderBuilder();

    ShaderBuilderNodes::Program& get() { return mProgram; }
    std::string getCode() const;

private:
    ShaderBuilderNodes::Program mProgram;
};