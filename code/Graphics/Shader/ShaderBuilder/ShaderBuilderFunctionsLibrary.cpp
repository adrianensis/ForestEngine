#include "Graphics/Shader/ShaderBuilder/ShaderBuilderFunctionsLibrary.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Material/Material.hpp"

void ShaderBuilderFunctionsLibrary::init(const ShaderBuilderNodes::Program& program, Ptr<const Material> material)
{
    registerFunctionCalculateSkinnedPosition(program, material);
}

void ShaderBuilderFunctionsLibrary::registerFunctionCalculateSkinnedPosition(const ShaderBuilderNodes::Program& program, Ptr<const Material> material)
{
    using namespace ShaderBuilderNodes;
    FunctionDefinition func(GPUBuiltIn::Functions::mCalculateSkinnedPosition);
    Variable pos = Variable(GPUBuiltIn::Functions::mCalculateSkinnedPosition.mParameters[0]);
    
    Variable finalPositon;
    Variable localPosition;
    
    func.body().
    variable(finalPositon, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "finalPositon", pos);

    if(material->getMaterialData().mIsSkinned)
    {
        auto& bonesIDs = program.getAttribute(GPUBuiltIn::VertexInput::mBonesIDs.mName);
        auto& bonesWeights = program.getAttribute(GPUBuiltIn::VertexInput::mBonesWeights.mName);
        auto& MAX_BONES = program.getAttribute(GPUBuiltIn::Consts::MAX_BONES.mName);
        auto& MAX_BONE_INFLUENCE = program.getAttribute(GPUBuiltIn::Consts::MAX_BONE_INFLUENCE.mName);
        auto& bonesMatricesblock = program.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices.mInstanceName);    
        Variable bonesTransform(bonesMatricesblock.mGPUSharedBufferData.getScopedGPUVariableData(0));

        func.body().
        set(finalPositon, call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {{"0.0f"}})).
        forBlock("i", "<", MAX_BONE_INFLUENCE, "++").
            ifBlock(bonesIDs.at("i"), "==", {"-1"}).
                line("continue").
            end().
            ifBlock(bonesIDs.at("i"), ">=", MAX_BONES).
                set(finalPositon, pos).
                line("break").
            end().
            variable(localPosition, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "localPosition", bonesTransform.at(bonesIDs.at("i")).mul(pos).getNameOrValue()).
            set(finalPositon, finalPositon.add(localPosition.mul(bonesWeights.at("i")))).
        end();
    }
    
    func.body().
    ret(finalPositon);

    mFunctions.insert_or_assign(func.mName, func);
}