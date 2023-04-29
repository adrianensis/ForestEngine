#include "Graphics/Shaders/ShaderBuilder.hpp"

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

    std::vector<std::string> Variable::toLines(u16 indent) const
    {
        std::string valueStr = mValue.empty() ? "" : " = " + mValue;
        std::string arrayStr = mArraySize.empty() ? "" : "[" + mArraySize + "]";
        return {getIndent(indent) + mType + " " + mName + arrayStr + valueStr + ";"};
    }

    std::vector<std::string> Attribute::toLines(u16 indent) const
    {
        std::string valueStr = mValue.empty() ? "" : " = " + mValue;
        std::string arrayStr = mArraySize.empty() ? "" : "[" + mArraySize + "]";
        std::string locationStr = mLocation < 0 ? "" : "layout (location=" + std::to_string(mLocation) + ") ";
        return {getIndent(indent) + locationStr + std::string(EnumsManager::toString(mGPUStorage)) + " " + mType + " " + mName + arrayStr + valueStr + ";"};
    }
    
    namespace Expressions
    {
        std::vector<std::string> Unary::toLines(u16 indent) const
        {
            if(mFront)
            {
                return {getIndent(indent) + mOp + mVariable.getNameOrValue()};
            }
            else
            {
                return {getIndent(indent) + mVariable.getNameOrValue() + mOp};
            }
        }

        std::vector<std::string> Binary::toLines(u16 indent) const
        {
            return {getIndent(indent) + mVariableA.getNameOrValue() + mOp + mVariableB.getNameOrValue()};
        }

        std::vector<std::string> Assign::toLines(u16 indent) const
        {
            return {getIndent(indent) + mVariableA.getNameOrValue() + mOp + mVariableB.getNameOrValue() + ";"};
        }
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
    BlockStatement& BlockStatement::ifBlock(const Variable& a, const std::string& op , const Variable& b)
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
            paramsStr += "in " + mParameters[i].mType + " " + mParameters[i].mName;

            if(i < (i32)mParameters.size() - 1)
            {
                paramsStr += ",";
            }
        }

        code.push_back(getIndent(indent) + mType + " " + mName + "(" + paramsStr + ")");
        auto statementCode = mBlockStatement.toLines(indent);
        code.insert(code.end(), statementCode.begin(), statementCode.end());
        return code;
    }

    Attribute& Program::attribute(GPUStorage GPUStorage, const Variable& var)
    {
        return mAttributes.emplace_back(Attribute(GPUStorage, var));
    }
    Attribute& Program::attribute(GPUStorage GPUStorage, u32 location, const Variable& var)
    {
        return mAttributes.emplace_back(Attribute(GPUStorage, location, var));
    }

    const Attribute& Program::getAttribute(const std::string_view& attributeName) const
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

    std::vector<std::string> Program::toLines(u16 indent) const
    {
        std::vector<std::string> code;

        code.push_back("#version " + std::to_string(mVersion));

        FOR_LIST(it, mAttributes)
        {
            auto statementCode = it->toLines(indent);
            code.insert(code.end(), statementCode.begin(), statementCode.end());
        }

        FOR_LIST(it, mFunctionDefinitions)
        {
            auto statementCode = it->toLines(indent);
            code.insert(code.end(), statementCode.begin(), statementCode.end());
        }

        return code;
    }
}