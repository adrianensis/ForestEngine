#pragma once

#include "Core/Module.hpp"

/*
    - none: (default) local read/write memory,
    or input parameter

    - const: global compile-time constant, or
    read-only function parameter,
    or read-only local variable

    - in: linkage into shader from previous stage

    - out: linkage out of a shader to next stage

    - attribute: same as in for vertex shader

    - uniform: linkage between a shader, OpenGL,
    and the application

    - varying: same as in for vertex shader, same as
    out for fragment shader
*/
enum class GPUStorage : u8
{
    NONE,
    IN,
    OUT,
    VARYING,
    ATTRIBUTE,
    CONST,
    UNIFORM
};

namespace ShaderBuilderNodes
{
    inline static std::string getGPUStorageString(GPUStorage as)
    {
        switch (as)
        {
            case GPUStorage::IN: return "in";
            case GPUStorage::OUT: return "out";
            case GPUStorage::VARYING: return "varying";
            case GPUStorage::ATTRIBUTE: return "attribute";
            case GPUStorage::CONST: return "const";
            case GPUStorage::UNIFORM: return "uniform";
            case GPUStorage::NONE: 
            default:
            return "";
        }
    }

    class Statement
    {
    public:
        Statement() = default;
        virtual ~Statement() = default;
        virtual std::string toString() const { std::vector<std::string> lines = toLines(0); return std::accumulate(lines.begin(), lines.end(), std::string("")); };
        virtual std::vector<std::string> toLines(u16 indent) const { return {"// none"}; };
    protected:
        static std::string getIndent(u16 indent);
    };

    class LineCode : public Statement
    {
    public:
        LineCode(const std::string& code) : mLine(code) {}
        std::vector<std::string> toLines(u16 indent) const override { return {getIndent(indent) + mLine + ";"}; };

        std::string mLine;
    };

    // VARIABLES DEFINITION

    class Variable : public Statement
    {
    public:
        Variable() = default;
        Variable(const std::string& value) : mValue(value) {};
        Variable(const std::string& type, const std::string& name) : mType(type), mName(name) {};
        Variable(const std::string& type, const std::string& name, const std::string& value) : mType(type), mName(name), mValue(value) {};
        Variable(const std::string& type, const std::string& name, const Variable& value) : Variable(type, name, value.getNameOrValue()) {};
        Variable(const std::string& type, const std::string& name, const std::string& value, const std::string& arraySize) : mType(type), mName(name), mValue(value), mArraySize(arraySize) {};
        Variable(const std::string& type, const std::string& name, const Variable& value, const Variable& arraySize) : Variable(type, name, value.getNameOrValue(), arraySize.getNameOrValue()) {};
        Variable(const std::string& type, const std::string& name, const std::string& value, const Variable& arraySize) : Variable(type, name, value, arraySize.getNameOrValue()) {};
        Variable(const std::string& type, const std::string& name, const Variable& value, const std::string& arraySize) : Variable(type, name, value.getNameOrValue(), arraySize) {};
        std::vector<std::string> toLines(u16 indent) const override;

        std::string getNameOrValue() const { return mName.empty() ? mValue : mName; }
        Variable at(const std::string& i) const { return Variable(getNameOrValue() + "[" + i + "]"); }
        Variable at(const Variable& i) const { return at(i.getNameOrValue()); }

        Variable binOp(const Variable& other, const std::string& op) const { return Variable(getNameOrValue() + op + other.getNameOrValue()); }
        Variable mul(const Variable& other) const { return binOp(other, "*"); }
        Variable div(const Variable& other) const { return binOp(other, "/"); }
        Variable add(const Variable& other) const { return binOp(other, "+"); }
        Variable sub(const Variable& other) const { return binOp(other, "-"); }

        std::string mType = "";
        std::string mName = "";
        std::string mValue = "";
        std::string mArraySize = "";
    };

    inline static Variable literal(const std::string& value) { return Variable(value); }
    inline static Variable call(const std::string& funcName, std::vector<Variable> params)
    {
        std::string callStr = funcName;
        callStr += "(";
        FOR_ARRAY(i, params)
        {
            callStr += params[i].getNameOrValue();

            if(i < params.size() - 1)
            {
                callStr += ",";
            }
        }
        callStr += ")";
        return Variable(callStr);
    }

    class Attribute : public Variable
    {
    public:
        Attribute(GPUStorage GPUStorage, const Variable& var) : Variable(var), mGPUStorage(GPUStorage) {};
        Attribute(GPUStorage GPUStorage, u32 location, const Variable& var) : Variable(var), mGPUStorage(GPUStorage), mLocation(location) {};

        std::vector<std::string> toLines(u16 indent) const override;

        i32 mLocation = -1;
        GPUStorage mGPUStorage = GPUStorage::NONE;
    };

    // EXPRESSIONS

    namespace Expressions
    {
        class ExpressionStatement: public Statement
        {
        };

        class Unary: public ExpressionStatement
        {
        public:
            Unary(const std::string& op, const Variable& var) : mOp(op), mVariable(var) {};
            Unary(const Variable& var, const std::string& op) : mFront(false), mOp(op), mVariable(var) {};
            std::vector<std::string> toLines(u16 indent) const override;

            std::string mOp = "??";
            Variable mVariable;
            bool mFront = true;
        };

        class Binary: public ExpressionStatement
        {
        public:
            Binary(const Variable& a, const std::string& op, const Variable& b) : mOp(op), mVariableA(a), mVariableB(b) {};
            std::vector<std::string> toLines(u16 indent) const override;

            std::string mOp = "??";
            Variable mVariableA;
            Variable mVariableB;
        };

        class Assign: public Binary
        {
        public:
            Assign(const Variable& a, const Variable& b) : Binary(a, "=", b) {};
            std::vector<std::string> toLines(u16 indent) const override;
        };
    }

    // STATEMENTS

    class IfStatement;
    class ForStatement;

    class BlockStatement : public Statement
    {
    public:
        Variable& variable(auto&& ...args)
        {
            Variable* newStatement = new Variable(args...);
            mStatements.push_back((Statement*)newStatement);
            return *newStatement;
        }
        BlockStatement& variable(Variable& out, auto&& ...args)
        {
            Variable* newStatement = new Variable(args...);
            out = *newStatement;
            mStatements.emplace_back((Statement*)newStatement);
            return *this;
        }
        BlockStatement& set(const Variable& a, const Variable& b);
        BlockStatement& set(const Variable& a, const std::string& value);
        BlockStatement& ifBlock(const Variable& a, const std::string& op , const Variable& b);
        BlockStatement& forBlock(auto&& ...args)
        {
            BlockStatement* newStatement = new ForStatement(args...);
            newStatement->mParent = this;
            mStatements.push_back((Statement*)newStatement);
            return *newStatement;
        }
        BlockStatement& line(const std::string& line);
        BlockStatement& end();

        std::vector<std::string> toLines(u16 indent) const override;

        BlockStatement* mParent = nullptr;
        std::vector<Statement*> mStatements;
    };

    class IfStatement : public BlockStatement
    {
    public:
        IfStatement(const Variable& a, const std::string& op , const Variable& b) : mExpression(a, op, b) {};

        std::vector<std::string> toLines(u16 indent) const override;

        Expressions::Binary mExpression;
    };

    class ForStatement : public BlockStatement
    {
    public:
        ForStatement(const std::string& varName, const std::string& op, const Variable& conditionVar, const std::string& advanceOp) :
        mVariable("int", varName, "0"), mConditionVariable(conditionVar), mContinueExpression(mVariable, op, mConditionVariable),
        mAdvanceExpression(mVariable, advanceOp) {};

        std::vector<std::string> toLines(u16 indent) const override;
        
        Variable mVariable;
        Variable mConditionVariable;
        Expressions::Binary mContinueExpression;
        Expressions::Unary mAdvanceExpression;
    };

    // FUNCTION DEFINITION

    class FunctionDefinition : public Statement
    {
    public:
        FunctionDefinition(const std::string& type, const std::string& name) : mType(type), mName(name) {};
        BlockStatement& body() { return mBlockStatement; };
        std::vector<std::string> toLines(u16 indent) const override;

        std::string mType = "??";
        std::string mName = "??";
        BlockStatement mBlockStatement;
    };

    class Program
    {
    public:
        Program()
        {
            mAttributes.reserve(100);
            mFunctionDefinitions.reserve(50);
        }
        Attribute& attribute(GPUStorage GPUStorage, const Variable& var);
        Attribute& attribute(GPUStorage GPUStorage, u32 location, const Variable& var);
        FunctionDefinition& function(auto&& ...args)
        {
            return mFunctionDefinitions.emplace_back(args...);
        }

        std::vector<std::string> toLines(u16 indent) const;

        std::vector<Attribute> mAttributes;
        std::vector<FunctionDefinition> mFunctionDefinitions;
        u16 mVersion = 420;
    };

}