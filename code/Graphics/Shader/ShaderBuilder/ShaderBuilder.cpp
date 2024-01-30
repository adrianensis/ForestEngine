#include "Graphics/Shader/ShaderBuilder/ShaderBuilder.hpp"

void ShaderBuilder::setVariableInCache(ShaderBuilderNodes::Variable& variable)
{
    CHECK_MSG(!mVariablesCache.contains(variable.getNameOrValue()), "Variable already found in cache!: " + variable.getNameOrValue());
    mVariablesCache.insert_or_assign(variable.getNameOrValue(), variable);
}

void ShaderBuilder::removeVariableFromCache(ShaderBuilderNodes::Variable& variable)
{
    CHECK_MSG(mVariablesCache.contains(variable.getNameOrValue()), "Variable not found in cache!: " + variable.getNameOrValue());
    mVariablesCache.erase(variable.getNameOrValue());
}

const ShaderBuilderNodes::Variable& ShaderBuilder::getVariableFromCache(const std::string& variableName) const
{
    CHECK_MSG(mVariablesCache.contains(variableName), "Variable not found in cache!: " + variableName);
    return mVariablesCache.at(variableName);
}

std::string ShaderBuilder::getCode() const
{
    std::string code = "";
    auto codeLines = mProgram.toLines(0);
    FOR_LIST(it, codeLines)
    {
        code.append((*it) + "\n");
    }

    return code;
}