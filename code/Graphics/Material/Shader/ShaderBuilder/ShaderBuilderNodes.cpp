#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"

namespace ShaderBuilderNodes
{
    std::string Statement::getIndent(u16 indent)
    {
        std::string str = "";
        FOR_RANGE(i, 0, indent)
        {
            str.append("    "); // four spaces
        }

        return str;
    };

    std::vector<std::string> Struct::toLines(u16 indent) const
    {
        std::vector<std::string> code;

        code.push_back(getIndent(indent) + "struct " + mStructDefinition.mName.get() + "{");

        FOR_LIST(it, mStructDefinition.mPrimitiveVariables)
        {
            Variable var(it->mGPUDataType, it->mName);
            auto varCode = var.toLines(indent + 1);
            code.insert(code.end(), varCode.begin(), varCode.end());
        }

        code.push_back(getIndent(indent) + "};");

        return code;
    }

    std::vector<std::string> Variable::toLines(u16 indent) const
    {
        std::string valueStr = mValue.empty() ? "" : " = " + mValue;
        std::string arrayStr = mArraySize.empty() ? "" : "[" + mArraySize + "]";
        return {getIndent(indent) + mType.mName.get() + " " + mName.get() + arrayStr + valueStr + ";"};
    }

    std::vector<std::string> Attribute::toLines(u16 indent) const
    {
        std::string valueStr = mValue.empty() ? "" : " = " + mValue;
        std::string arrayStr = mArraySize.empty() ? "" : "[" + mArraySize + "]";
        std::string layoutStr = "";
        std::string locationStr = mLocationOrBinding < 0 ? "" : "location=" + std::to_string(mLocationOrBinding);
        if(mGPUStorage == GPUStorage::UNIFORM)
        {
            locationStr = mLocationOrBinding < 0 ? "" : "binding=" + std::to_string(mLocationOrBinding);
        }
        std::string setStr = mSet < 0 ? "" : "set=" + std::to_string(mSet);
        if(mGPUStorage == GPUStorage::UNIFORM || mGPUStorage == GPUStorage::IN || mGPUStorage == GPUStorage::OUT)
        {
            layoutStr = "layout(" + (setStr.empty() ? setStr : setStr + ",") + locationStr + ")";
        }
        std::string interpolationStr = mGPUInterpolation == GPUInterpolation::NONE ? "" : EnumsManager::toString(mGPUInterpolation).get() + " ";
        std::string storageStr = EnumsManager::toString(mGPUStorage).get() + " ";
        return {getIndent(indent) + layoutStr + interpolationStr + storageStr + mType.mName.get() + " " + mName.get() + arrayStr + valueStr + ";"};
    }
    
    std::vector<std::string> UniformBuffer::toLines(u16 indent) const
    {
        std::vector<std::string> code;

        std::string bindingStr = mBinding < 0 ? "" : ", binding=" + std::to_string(mBinding);

        std::string layoutStr;
        switch (mGPUUniformBufferData.mType)
        {
        case GPUBufferType::UNIFORM:
            layoutStr = "layout (std140"+ bindingStr +") uniform";
            break;
        case GPUBufferType::STORAGE:
            // layoutStr = "layout (std430"+ bindingStr +") readonly buffer";
            layoutStr = "layout (std430"+ bindingStr +") buffer";
            break;
        default:
            CHECK_MSG(false, "Ilegal GPUBufferType!");
            break;
        }

        code.push_back(getIndent(indent) + layoutStr + " " + mGPUUniformBufferData.mBufferName.get() + " {");

        const auto& variableDefinitionDataArray = mGPUUniformBufferData.mGPUVariableDefinitionDataArray;
        FOR_LIST(it, variableDefinitionDataArray)
        {
            const GPUVariableDefinitionData& variableDefinitionData = *it;
            const Variable variable(variableDefinitionData);
            auto statementCode = variable.toLines(indent + 1);
            code.insert(code.end(), statementCode.begin(), statementCode.end());
        }

        code.push_back(getIndent(indent) + "} " + mGPUUniformBufferData.mInstanceName.get() + ";");

        return code;
    }
    
    namespace Expressions
    {
        std::vector<std::string> Unary::toLines(u16 indent) const
        {
            if(mFront)
            {
                return {getIndent(indent) + mOp.get() + mVariable.getNameOrValue()};
            }
            else
            {
                return {getIndent(indent) + mVariable.getNameOrValue() + mOp.get()};
            }
        }

        std::vector<std::string> Binary::toLines(u16 indent) const
        {
            return {getIndent(indent) + mVariableA.getNameOrValue() + mOp.get() + mVariableB.getNameOrValue()};
        }

        std::vector<std::string> Assign::toLines(u16 indent) const
        {
            return {getIndent(indent) + mVariableA.getNameOrValue() + mOp.get() + mVariableB.getNameOrValue() + ";"};
        }
    }

    void BlockStatement::terminate()
    {
        FOR_LIST(it, mStatements)
        {
            delete *it;
        }
        mStatements.clear();
    }

    BlockStatement& BlockStatement::set(const Variable& a, const Variable& b)
    {
        Statement* newStatement = new Expressions::Assign(a, b);
        mStatements.push_back((Statement*)newStatement);
        return *this;
    }
    BlockStatement& BlockStatement::set(const Variable& a, const std::string& value)
    {
        return set(a, Variable(value));
    }
    BlockStatement& BlockStatement::ifBlock(const Variable& a, const HashedString& op , const Variable& b)
    {
        BlockStatement* newStatement = new IfStatement(a, op, b);
        newStatement->mParent = this;
        mStatements.push_back((Statement*)newStatement);
        return *newStatement;
    }
    BlockStatement& BlockStatement::ifBlock(const Variable& boolean)
    {
        BlockStatement* newStatement = new IfStatement(boolean);
        newStatement->mParent = this;
        mStatements.push_back((Statement*)newStatement);
        return *newStatement;
    }
    BlockStatement& BlockStatement::elseBlock()
    {
        BlockStatement* newStatement = new ElseStatement();
        newStatement->mParent = this;
        mStatements.push_back((Statement*)newStatement);
        return *newStatement;
    }
    BlockStatement& BlockStatement::line(const std::string& line)
    {
        Statement* newStatement = new LineCode(line);
        mStatements.push_back((Statement*)newStatement);
        return *this;
    }
    BlockStatement& BlockStatement::ret(const Variable& returnValue)
    {
        return line("return " + returnValue.getNameOrValue());
    }
    BlockStatement& BlockStatement::end()
    {
        return *mParent;
    }

    std::vector<std::string> BlockStatement::toLines(u16 indent) const
    {
        std::vector<std::string> code;

        code.push_back(getIndent(indent) + "{");

        FOR_LIST(it, mStatements)
        {
            auto statementCode = (*it)->toLines(indent + 1);
            code.insert(code.end(), statementCode.begin(), statementCode.end());
        }

        code.push_back(getIndent(indent) + "}");

        return code;
    }

    std::vector<std::string> IfStatement::toLines(u16 indent) const
    {
        std::vector<std::string> code;

        code.push_back(getIndent(indent) + "if(" + mExpression.toString() + ")");
        auto statementCode = BlockStatement::toLines(indent);
        code.insert(code.end(), statementCode.begin(), statementCode.end());
        return code;
    }

    std::vector<std::string> ElseStatement::toLines(u16 indent) const
    {
        std::vector<std::string> code;

        code.push_back(getIndent(indent) + "else");
        auto statementCode = BlockStatement::toLines(indent);
        code.insert(code.end(), statementCode.begin(), statementCode.end());
        return code;
    }

    std::vector<std::string> ForStatement::toLines(u16 indent) const
    {
        std::vector<std::string> code;

        code.push_back(getIndent(indent) + "for(" + mVariable.toString() + mContinueExpression.toString() + ";" + mAdvanceExpression.toString() + ")");
        auto statementCode = BlockStatement::toLines(indent);
        code.insert(code.end(), statementCode.begin(), statementCode.end());
        return code;
    }

    std::vector<std::string> FunctionDefinition::toLines(u16 indent) const
    {
        std::vector<std::string> code;

        std::string paramsStr = "";
        
        FOR_ARRAY(i, mParameters)
        {
            paramsStr += "in " + mParameters[i].mType.mName.get() + " " + mParameters[i].mName.get();

            if(i < (i32)mParameters.size() - 1)
            {
                paramsStr += ",";
            }
        }

        code.push_back(getIndent(indent) + mType.mName.get() + " " + mName.get() + "(" + paramsStr + ")");
        auto statementCode = mBlockStatement.toLines(indent);
        code.insert(code.end(), statementCode.begin(), statementCode.end());
        return code;
    }
    
    Struct& Program::structType(const Struct& structType)
    {
        return mStructs.emplace_back(structType);
    }

    Attribute& Program::attribute(const Attribute& attribute)
    {
        return mAttributes.emplace_back(attribute);
    }

    UniformBuffer& Program::uniformBuffer(const UniformBuffer& uniformBuffer)
    {
        return mUniformBuffers.emplace_back(uniformBuffer);
    }
    
    const Struct& Program::getStruct(const HashedString& structName) const
    {
        FOR_LIST(it, mStructs)
        {
            if(it->mStructDefinition.mName == structName)
            {
                return *it;
            }
        }

        return mNullStructDefinition;
    }

    const Attribute& Program::getAttribute(const HashedString& attributeName) const
    {
        FOR_LIST(it, mAttributes)
        {
            if(it->mName == attributeName)
            {
                return *it;
            }
        }

        return mNullAttribute;
    }

    const UniformBuffer& Program::getUniformBuffer(const HashedString& uniformBufferName) const
    {
        FOR_LIST(it, mUniformBuffers)
        {
            if(it->mGPUUniformBufferData.mInstanceName == uniformBufferName)
            {
                return *it;
            }
        }

        return mNullUniformBuffer;
    }

    FunctionDefinition& Program::getFunctionDefinition(const HashedString& functionDefinitionName)
    {
        FOR_LIST(it, mFunctionDefinitions)
        {
            if(it->mName == functionDefinitionName)
            {
                return *it;
            }
        }

        return mNullFunctionDefinition;
    }

    const Struct& Program::getStruct(const Struct& structType) const
    {
        return getStruct(structType.mStructDefinition.mName);
    }

    const Attribute& Program::getAttribute(const Attribute& attribute) const
    {
        return getAttribute(attribute.mName);
    }

    const UniformBuffer& Program::getUniformBuffer(const UniformBuffer& uniformBuffer) const
    {
        return getUniformBuffer(uniformBuffer.mGPUUniformBufferData.mInstanceName);
    }

    FunctionDefinition& Program::getFunctionDefinition(const GPUFunctionDefinition& functionDefinition)
    {
        return getFunctionDefinition(functionDefinition.mName);
    }

    std::vector<std::string> Program::toLines(u16 indent) const
    {
        std::vector<std::string> code;

        code.push_back("#version " + std::to_string(mVersion));

        FOR_LIST(it, mExtensions)
        {
            code.push_back("#extension " + it->get() + " : require");
        }

        FOR_LIST(it, mStructs)
        {
            auto statementCode = it->toLines(indent);
            code.insert(code.end(), statementCode.begin(), statementCode.end());
        }

        FOR_LIST(it, mAttributes)
        {
            auto statementCode = it->toLines(indent);
            code.insert(code.end(), statementCode.begin(), statementCode.end());
        }

        FOR_LIST(it, mUniformBuffers)
        {
            auto statementCode = it->toLines(indent);
            code.insert(code.end(), statementCode.begin(), statementCode.end());
        }

        FOR_LIST(it, mFunctionDefinitions)
        {
            auto statementCode = it->toLines(indent);
            code.insert(code.end(), statementCode.begin(), statementCode.end());
        }

        auto statementCode = mMainFunctionDefinition.toLines(indent);
        code.insert(code.end(), statementCode.begin(), statementCode.end());

        return code;
    }

    void Program::terminate()
    {
        FOR_LIST(it, mStructs) { it->terminate(); }
        FOR_LIST(it, mAttributes) { it->terminate(); }
        FOR_LIST(it, mUniformBuffers) { it->terminate(); }
        FOR_LIST(it, mFunctionDefinitions) { it->terminate(); }
        mMainFunctionDefinition.terminate();
    }

}