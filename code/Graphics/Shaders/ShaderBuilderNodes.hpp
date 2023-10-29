#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUSharedBlock.hpp"

namespace ShaderBuilderNodes
{
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

    class VariableData
    {
    public:
        VariableData() = default;
        VariableData(GPUDataType gpuDataType, std::string name, std::string value, std::string arraySize):
        mGPUDataType(gpuDataType),
        mName(name),
        mValue(value),
        mArraySize(arraySize)
        {}
        VariableData(GPUDataType gpuDataType, std::string name, std::string value):
        mGPUDataType(gpuDataType),
        mName(name),
        mValue(value)
        {}
        VariableData(GPUDataType gpuDataType, std::string name):
        mGPUDataType(gpuDataType),
        mName(name)
        {}

        GPUDataType mGPUDataType;
        std::string mName;
        std::string mValue;
        std::string mArraySize;
    };

    class Variable : public Statement
    {
    public:
        Variable() = default;
        Variable(const VariableData& VariableData) : mType(VariableData.mGPUDataType.mName), mName(VariableData.mName), mValue(VariableData.mValue), mArraySize(VariableData.mArraySize) {};
        Variable(const VariableData& VariableData, const std::string& value) : mType(VariableData.mGPUDataType.mName), mName(VariableData.mName), mValue(value), mArraySize(VariableData.mArraySize) {};
        Variable(const VariableData& VariableData, const Variable& value) : mType(VariableData.mGPUDataType.mName), mName(VariableData.mName), mValue(value.getNameOrValue()), mArraySize(VariableData.mArraySize) {};
        Variable(const std::string& value) : mValue(value) {};
        Variable(const std::string& type, const std::string& name) : mType(type), mName(name) {};
        Variable(const std::string& type, const std::string& name, const std::string& value) : mType(type), mName(name), mValue(value) {};
        Variable(const std::string& type, const std::string& name, const Variable& value) : Variable(type, name, value.getNameOrValue()) {};
        Variable(const std::string& type, const std::string& name, const std::string& value, const std::string& arraySize) : mType(type), mName(name), mValue(value), mArraySize(arraySize) {};
        Variable(const std::string& type, const std::string& name, const Variable& value, const Variable& arraySize) : Variable(type, name, value.getNameOrValue(), arraySize.getNameOrValue()) {};
        Variable(const std::string& type, const std::string& name, const std::string& value, const Variable& arraySize) : Variable(type, name, value, arraySize.getNameOrValue()) {};
        Variable(const std::string& type, const std::string& name, const Variable& value, const std::string& arraySize) : Variable(type, name, value.getNameOrValue(), arraySize) {};
        Variable(const GPUVariableDefinitionData& gpuVariableData) : mType(gpuVariableData.mGPUDataType.mName), mName(gpuVariableData.mName), mValue(gpuVariableData.mValue), mArraySize(gpuVariableData.mArraySize){};
        std::vector<std::string> toLines(u16 indent) const override;

        const std::string& getNameOrValue() const { return mName.empty() ? mValue : mName; }
        bool isEmpty() const {return getNameOrValue().empty(); };
        Variable dot(const std::string& member) const { return Variable(getNameOrValue() + "." + member); }

        Variable at(const std::string& i) const { return Variable(getNameOrValue() + "[" + i + "]"); }
        Variable at(const Variable& i) const { return at(i.getNameOrValue()); }

        Variable binOp(const Variable& other, const std::string& op) const { return Variable(getNameOrValue() + op + other.getNameOrValue()); }
        Variable mul(const Variable& other) const { return binOp(other, "*"); }
        Variable div(const Variable& other) const { return binOp(other, "/"); }
        Variable add(const Variable& other) const { return binOp(other, "+"); }
        Variable sub(const Variable& other) const { return binOp(other, "-"); }
        Variable eq(const Variable& other) const { return binOp(other, "=="); }
        Variable notEq(const Variable& other) const { return binOp(other, "!="); }

        std::string mType = "";
        std::string mName = "";
        std::string mValue = "";
        std::string mArraySize = "";
    };

    inline static Variable literal(const std::string& value) { return Variable(value); }
    inline static Variable call(const std::string& funcName, const std::vector<Variable>& params)
    {
        std::string callStr = funcName;
        callStr += "(";
        FOR_ARRAY(i, params)
        {
            callStr += params[i].getNameOrValue();

            if((u32)(i) < params.size() - 1)
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
        Attribute(const GPUVariableDefinitionData& gpuVariableData) : Variable(gpuVariableData), mGPUStorage(gpuVariableData.mGPUStorage) {};
        Attribute(const GPUVariableDefinitionData& gpuVariableData, u32 location) : Variable(gpuVariableData), mGPUStorage(gpuVariableData.mGPUStorage), mLocation(location) {};

        std::vector<std::string> toLines(u16 indent) const override;

        GPUStorage mGPUStorage = GPUStorage::NONE;
        i32 mLocation = -1;
    };

    class AttributeBlock : public Statement
    {
    public:
        AttributeBlock(const GPUSharedBlockData& gpuBlockData) : mGPUSharedBlockData(gpuBlockData) {};

        std::vector<std::string> toLines(u16 indent) const override;

        GPUSharedBlockData mGPUSharedBlockData;
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
            Unary(const Variable& var, const std::string& op) : mOp(op), mVariable(var), mFront(false) {};
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
    class ElseStatement;
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
        BlockStatement& ifBlock(const Variable& boolean);
        BlockStatement& elseBlock();
        BlockStatement& forBlock(auto&& ...args)
        {
            BlockStatement* newStatement = new ForStatement(args...);
            newStatement->mParent = this;
            mStatements.push_back((Statement*)newStatement);
            return *newStatement;
        }
        BlockStatement& line(const std::string& line);
        BlockStatement& ret(const Variable& a);
        BlockStatement& end();

        std::vector<std::string> toLines(u16 indent) const override;

        BlockStatement* mParent = nullptr;
        std::vector<Statement*> mStatements;
    };

    class IfStatement : public BlockStatement
    {
    public:
        IfStatement(const Variable& a, const std::string& op , const Variable& b) : mExpression(a, op, b) {};
        IfStatement(const Variable& boolean) : mExpression(boolean, "", {}) {};

        std::vector<std::string> toLines(u16 indent) const override;

        Expressions::Binary mExpression;
    };

    class ElseStatement : public BlockStatement
    {
    public:
        ElseStatement() {};

        std::vector<std::string> toLines(u16 indent) const override;
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
        FunctionDefinition(const std::string& type, const std::string& name, const std::vector<Variable>& params) : mType(type), mName(name), mParameters(params) {};
        BlockStatement& body() { return mBlockStatement; };
        std::vector<std::string> toLines(u16 indent) const override;

        std::string mType = "??";
        std::string mName = "??";
        std::vector<Variable> mParameters;
        BlockStatement mBlockStatement;
    };

    class Program
    {
    public:
        Program()
        {
            mAttributes.reserve(100);
            mAttributeBlocks.reserve(50);
            mFunctionDefinitions.reserve(50);
        }
        Attribute& attribute(const Attribute& attribute);
        AttributeBlock& attributeBlock(const AttributeBlock& attributeBlock);
        FunctionDefinition& function(auto&& ...args)
        {
            return mFunctionDefinitions.emplace_back(args...);
        }

        const Attribute& getAttribute(const std::string_view& attributeName) const;
        const AttributeBlock& getAttributeBlock(const std::string_view& attributeBlockName) const;

        std::vector<std::string> toLines(u16 indent) const;

        std::vector<Attribute> mAttributes;
        std::vector<AttributeBlock> mAttributeBlocks;
        std::vector<FunctionDefinition> mFunctionDefinitions;
        u16 mVersion = 420;
    private:
        inline static Attribute mNullAttribute {GPUVariableDefinitionData{}};
        inline static AttributeBlock mNullAttributeBlock {GPUSharedBlockData{}};
    };

}