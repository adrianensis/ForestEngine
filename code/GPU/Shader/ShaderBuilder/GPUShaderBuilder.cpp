#include "GPU/Shader/ShaderBuilder/GPUShaderBuilder.hpp"

GPUShaderBuilder::~GPUShaderBuilder()
{
    mProgram.terminate();
}

GPUShaderBuilder::GPUShaderBuilder()
{
    auto& mainFunc = mProgram.mainFunction(GPUShaderDefinitions::Functions::mMain);
}

GPUShaderBuilderNodes::BlockStatement& GPUShaderBuilder::getMain()
{
    auto& mainFunc = mProgram.getMainFunctionDefinition();
    return mainFunc.body();
}

void GPUShaderBuilder::setVariableInCache(GPUShaderBuilderNodes::Variable& variable)
{
    CHECK_MSG(!mVariablesCache.contains(variable.getNameOrValue()), "Variable already found in cache!: " + variable.getNameOrValue());
    mVariablesCache.insert_or_assign(variable.getNameOrValue(), variable);
}

void GPUShaderBuilder::removeVariableFromCache(GPUShaderBuilderNodes::Variable& variable)
{
    CHECK_MSG(mVariablesCache.contains(variable.getNameOrValue()), "Variable not found in cache!: " + variable.getNameOrValue());
    mVariablesCache.erase(variable.getNameOrValue());
}

const GPUShaderBuilderNodes::Variable& GPUShaderBuilder::getVariableFromCache(const std::string& variableName) const
{
    CHECK_MSG(mVariablesCache.contains(variableName), "Variable not found in cache!: " + variableName);
    return mVariablesCache.at(variableName);
}

std::string GPUShaderBuilder::getCode() const
{
    std::string code = "";
    auto codeLines = mProgram.toLines(0);
    FOR_LIST(it, codeLines)
    {
        code.append((*it) + "\n");
    }

    return code;
}