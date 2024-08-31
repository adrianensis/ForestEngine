#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUUniformBuffer.hpp"

namespace ShaderBuilderNodes
{
    class Statement
    {
    public:
        Statement() = default;
        virtual ~Statement() = default;
        virtual void terminate() {};
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
        VariableData(GPUDataType gpuDataType, HashedString name, std::string value, std::string arraySize):
        mGPUDataType(gpuDataType),
        mName(name),
        mValue(value),
        mArraySize(arraySize)
        {}
        VariableData(GPUDataType gpuDataType, HashedString name, std::string value):
        mGPUDataType(gpuDataType),
        mName(name),
        mValue(value)
        {}
        VariableData(GPUDataType gpuDataType, HashedString name):
        mGPUDataType(gpuDataType),
        mName(name)
        {}

        GPUDataType mGPUDataType;
        HashedString mName;
        std::string mValue;
        std::string mArraySize;
    };

    class Struct : public Statement
    {
    public:
        Struct(const GPUStructDefinition& structDefinition): mStructDefinition(structDefinition) {};
        std::vector<std::string> toLines(u16 indent) const override;

        GPUStructDefinition mStructDefinition;
    };

    class Variable : public Statement
    {
    public:
        Variable() = default;
        Variable(const VariableData& VariableData) : mType(VariableData.mGPUDataType), mName(VariableData.mName), mValue(VariableData.mValue), mArraySize(VariableData.mArraySize) {};
        Variable(const VariableData& VariableData, const std::string& value) : mType(VariableData.mGPUDataType), mName(VariableData.mName), mValue(value), mArraySize(VariableData.mArraySize) {};
        Variable(const VariableData& VariableData, const Variable& value) : mType(VariableData.mGPUDataType), mName(VariableData.mName), mValue(value.getNameOrValue()), mArraySize(VariableData.mArraySize) {};
        Variable(const std::string& value) : mValue(value) {};
        Variable(const GPUDataType& type, const HashedString& name) : mType(type), mName(name) {};
        Variable(const GPUDataType& type, const HashedString& name, const std::string& value) : mType(type), mName(name), mValue(value) {};
        Variable(const GPUDataType& type, const HashedString& name, const Variable& value) : Variable(type, name, value.getNameOrValue()) {};
        Variable(const GPUDataType& type, const HashedString& name, const std::string& value, const std::string& arraySize) : mType(type), mName(name), mValue(value), mArraySize(arraySize) {};
        Variable(const GPUDataType& type, const HashedString& name, const Variable& value, const Variable& arraySize) : Variable(type, name, value.getNameOrValue(), arraySize.getNameOrValue()) {};
        Variable(const GPUDataType& type, const HashedString& name, const std::string& value, const Variable& arraySize) : Variable(type, name, value, arraySize.getNameOrValue()) {};
        Variable(const GPUDataType& type, const HashedString& name, const Variable& value, const std::string& arraySize) : Variable(type, name, value.getNameOrValue(), arraySize) {};
        Variable(const GPUVariableDefinitionData& gpuVariableData) : mType(gpuVariableData.mGPUDataType), mName(gpuVariableData.mName), mValue(gpuVariableData.mValue), mArraySize(gpuVariableData.mArraySize){};
        Variable(const GPUStructDefinition::GPUStructVariable& gpuStructVariableData) : mType(gpuStructVariableData.mGPUDataType), mName(gpuStructVariableData.mName) {};
        std::vector<std::string> toLines(u16 indent) const override;

        const std::string& getNameOrValue() const { return mName.isValid() ? mName.get() : mValue; }
        bool isEmpty() const {return getNameOrValue().empty(); };
        bool isValid() const {return !isEmpty(); };
        Variable dot(const HashedString& other) const { return Variable(getNameOrValue() + "." + other.get()); }
        Variable dot(const Variable& other) const { return Variable(getNameOrValue() + "." + other.getNameOrValue()); }

        Variable at(const HashedString& i) const { return Variable(getNameOrValue() + "[" + i.get() + "]"); }
        Variable at(const Variable& i) const { return Variable(getNameOrValue() + "[" + i.getNameOrValue() + "]"); }

        Variable binOp(const Variable& other, const HashedString& op) const { return Variable(getNameOrValue() + op.get() + other.getNameOrValue()); }
        Variable mul(const Variable& other) const { return binOp(other, "*"); }
        Variable div(const Variable& other) const { return binOp(other, "/"); }
        Variable add(const Variable& other) const { return binOp(other, "+"); }
        Variable sub(const Variable& other) const { return binOp(other, "-"); }
        Variable eq(const Variable& other) const { return binOp(other, "=="); }
        Variable notEq(const Variable& other) const { return binOp(other, "!="); }
        Variable great(const Variable& other) const { return binOp(other, ">"); }
        Variable greatEq(const Variable& other) const { return binOp(other, ">="); }
        Variable less(const Variable& other) const { return binOp(other, "<"); }
        Variable lessEq(const Variable& other) const { return binOp(other, "<="); }
        Variable neg() const { return Variable("-" + getNameOrValue()); }
        Variable ternary(const Variable& other1, const Variable& other2) const { return Variable(getNameOrValue()+"?" + other1.binOp(other2, ":").getNameOrValue()); }

        GPUDataType mType;
        HashedString mName;
        std::string mValue = "";
        std::string mArraySize = "";
    };

    inline static Variable literal(const std::string& value) { return Variable(value); }
    inline static Variable call(const HashedString& funcName, const std::vector<Variable>& params)
    {
        std::string callStr = funcName.get();
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
    inline static Variable call(const GPUDataType& dataType, const std::vector<Variable>& params)
    {
        return call(dataType.mName, params);
    }
    inline static Variable call(const GPUFunctionDefinition& funcDef, const std::vector<Variable>& params)
    {
        return call(funcDef.mName, params);
    }
    inline static Variable paren(const Variable& variable)
    {
        return Variable("("+variable.getNameOrValue()+")");
    }

    class Attribute : public Variable
    {
    public:
        Attribute(const GPUVariableDefinitionData& gpuVariableData) : Variable(gpuVariableData), mGPUInterpolation(gpuVariableData.mGPUInterpolation), mGPUStorage(gpuVariableData.mGPUStorage) {};
        Attribute(const GPUVariableDefinitionData& gpuVariableData, u32 locationOrBinding) : Variable(gpuVariableData), mGPUInterpolation(gpuVariableData.mGPUInterpolation), mGPUStorage(gpuVariableData.mGPUStorage), mLocationOrBinding(locationOrBinding) {};
        Attribute(const GPUVariableDefinitionData& gpuVariableData, u32 locationOrBinding, u32 set) : Variable(gpuVariableData), mGPUInterpolation(gpuVariableData.mGPUInterpolation), mGPUStorage(gpuVariableData.mGPUStorage), mLocationOrBinding(locationOrBinding), mSet(set) {};

        std::vector<std::string> toLines(u16 indent) const override;

        GPUInterpolation mGPUInterpolation = GPUInterpolation::NONE;
        GPUStorage mGPUStorage = GPUStorage::NONE;
        i32 mLocationOrBinding = INVALID_INDEX;
        i32 mSet = INVALID_INDEX;
    };

    class UniformBuffer : public Statement
    {
    public:
        UniformBuffer() {};
        UniformBuffer(const GPUUniformBufferData& gpuBlockData, u32 binding) : mGPUUniformBufferData(gpuBlockData), mBinding(binding) {};

        std::vector<std::string> toLines(u16 indent) const override;
        bool isValid() const { return mGPUUniformBufferData.mInstanceName.isValid(); }

        GPUUniformBufferData mGPUUniformBufferData;
        i32 mBinding = INVALID_INDEX;
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
            Unary(const HashedString& op, const Variable& var) : mOp(op), mVariable(var) {};
            Unary(const Variable& var, const HashedString& op) : mOp(op), mVariable(var), mFront(false) {};
            std::vector<std::string> toLines(u16 indent) const override;

            HashedString mOp;
            Variable mVariable;
            bool mFront = true;
        };

        class Binary: public ExpressionStatement
        {
        public:
            Binary(const Variable& a, const HashedString& op, const Variable& b) : mOp(op), mVariableA(a), mVariableB(b) {};
            std::vector<std::string> toLines(u16 indent) const override;

            HashedString mOp;
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
        BlockStatement& ifBlock(const Variable& a, const HashedString& op , const Variable& b);
        BlockStatement& ifBlock(const Variable& boolean);
        BlockStatement& elseBlock();
        BlockStatement& forBlock(auto&& ...args);
        BlockStatement& line(const std::string& line);
        BlockStatement& ret(const Variable& a);
        BlockStatement& end();

        std::vector<std::string> toLines(u16 indent) const override;
        void terminate() override;

        BlockStatement* mParent = nullptr;
        std::vector<Statement*> mStatements;
    };

    class IfStatement : public BlockStatement
    {
    public:
        IfStatement(const Variable& a, const HashedString& op , const Variable& b) : mExpression(a, op, b) {};
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
        ForStatement(const HashedString& varName, const HashedString& op, const Variable& conditionVar, const HashedString& advanceOp) :
        mVariable(GPUBuiltIn::PrimitiveTypes::mInt, varName, "0"), mConditionVariable(conditionVar), mContinueExpression(mVariable, op, mConditionVariable),
        mAdvanceExpression(mVariable, advanceOp) {};

        std::vector<std::string> toLines(u16 indent) const override;
        
        Variable mVariable;
        Variable mConditionVariable;
        Expressions::Binary mContinueExpression;
        Expressions::Unary mAdvanceExpression;
    };

    BlockStatement& BlockStatement::forBlock(auto&& ...args)
    {
        BlockStatement* newStatement = new ForStatement(args...);
        newStatement->mParent = this;
        mStatements.push_back((Statement*)newStatement);
        return *newStatement;
    }

    // FUNCTION DEFINITION

    class FunctionDefinition : public Statement
    {
    public:
        FunctionDefinition(const GPUFunctionDefinition& gpuFunctionDefinition) : mType(gpuFunctionDefinition.mType), mName(gpuFunctionDefinition.mName)
        {
            FOR_ARRAY(i, gpuFunctionDefinition.mParameters)
            {
                mParameters.push_back(GPUVariableDefinitionData(gpuFunctionDefinition.mParameters[i]));
            }
        };
        BlockStatement& body() { return mBlockStatement; };
        std::vector<std::string> toLines(u16 indent) const override;

        GPUDataType mType;
        HashedString mName;
        std::vector<Variable> mParameters;
        BlockStatement mBlockStatement;
    };

    class Program
    {
    public:
        Program()
        {
            mStructs.reserve(50);
            mAttributes.reserve(100);
            mUniformBuffers.reserve(50);
            mFunctionDefinitions.reserve(50);
        }
        ~Program() { terminate(); }
        
        Struct& structType(const Struct& structType);
        Attribute& attribute(const Attribute& attribute);
        UniformBuffer& uniformBuffer(const UniformBuffer& uniformBuffer);
        FunctionDefinition& mainFunction(auto&& ...args)
        {
            return mMainFunctionDefinition = FunctionDefinition(args...);
        }
        FunctionDefinition& function(auto&& ...args)
        {
            return mFunctionDefinitions.emplace_back(args...);
        }

        void extension(HashedString ext)
        {
            mExtensions.push_back(ext);
        }

        const Struct& getStruct(const HashedString& structName) const;
        const Struct& getStruct(const Struct& structType) const;
        const Attribute& getAttribute(const HashedString& attributeName) const;
        const Attribute& getAttribute(const Attribute& attribute) const;
        const UniformBuffer& getUniformBuffer(const HashedString& uniformBufferName) const;
        const UniformBuffer& getUniformBuffer(const UniformBuffer& uniformBuffer) const;
        FunctionDefinition& getFunctionDefinition(const HashedString& functionDefinitionName);
        FunctionDefinition& getFunctionDefinition(const GPUFunctionDefinition& functionDefinition);
        FunctionDefinition& getMainFunctionDefinition()
        {
            return mMainFunctionDefinition;
        }

        std::vector<std::string> toLines(u16 indent) const;
        void terminate();

        std::vector<Struct> mStructs;
        std::vector<Attribute> mAttributes;
        std::vector<UniformBuffer> mUniformBuffers;
        std::vector<FunctionDefinition> mFunctionDefinitions;
        std::vector<HashedString> mExtensions;
        FunctionDefinition mMainFunctionDefinition = GPUFunctionDefinition{};
        u16 mVersion = 460;
    private:
        inline static Attribute mNullAttribute {GPUVariableDefinitionData{}};
        inline static UniformBuffer mNullUniformBuffer {};
        inline static Struct mNullStructDefinition {GPUStructDefinition{}};
        inline static FunctionDefinition mNullFunctionDefinition {GPUFunctionDefinition{}};
    };

}