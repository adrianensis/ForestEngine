#include "Graphics/ShaderBuilder/ShaderBuilderFunctionsLibrary.hpp"
#include "Graphics/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Material/Material.hpp"

ShaderBuilderNodes::FunctionDefinition ShaderBuilderFunctionsLibrary::getFunctionCalculateSkinnedPosition(const ShaderBuilderNodes::Program& program, Ptr<const Material> material)
{
    using namespace ShaderBuilderNodes;
    Variable pos = Variable(GPUBuiltIn::PrimitiveTypes::mVector4.mName, "pos");
    FunctionDefinition func(GPUBuiltIn::PrimitiveTypes::mVector4.mName, "calculateSkinnedPosition", {pos});
    
    Variable finalPositon;
    Variable localPosition;
    
    func.body().
    variable(finalPositon, "vec4", "finalPositon", pos);

    if(material->getMaterialData().mIsSkinned)
    {
        auto& bonesIDs = program.getAttribute(GPUBuiltIn::VertexInput::mBonesIDs.mName);
        auto& bonesWeights = program.getAttribute(GPUBuiltIn::VertexInput::mBonesWeights.mName);
        auto& MAX_BONES = program.getAttribute(GPUBuiltIn::Consts::MAX_BONES.mName);
        auto& MAX_BONE_INFLUENCE = program.getAttribute(GPUBuiltIn::Consts::MAX_BONE_INFLUENCE.mName);
        auto& bonesMatricesblock = program.getAttributeBlock(GPUBuiltIn::UniformBlocks::mBonesMatrices.mInstanceName);    
        Variable bonesTransform(bonesMatricesblock.mGPUUniformBlockData.getScopedGPUVariableData(0));

        func.body().
        set(finalPositon, call("vec4", {{"0.0f"}})).
        forBlock("i", "<", MAX_BONE_INFLUENCE, "++").
            ifBlock(bonesIDs.at("i"), "==", {"-1"}).
                line("continue").
            end().
            ifBlock(bonesIDs.at("i"), ">=", MAX_BONES).
                set(finalPositon, pos).
                line("break").
            end().
            variable(localPosition, "vec4", "localPosition", bonesTransform.at(bonesIDs.at("i")).mul(pos).getNameOrValue()).
            set(finalPositon, finalPositon.add(localPosition.mul(bonesWeights.at("i")))).
        end();
    }
    
    func.body().
    ret(finalPositon);

    return func;
}