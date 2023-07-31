#include "Graphics/Shaders/ShaderBuilderFunctionsLibrary.hpp"
#include "Graphics/Shaders/ShaderBuilderNodes.hpp"
#include "Graphics/Buffers/GPUBuiltIn.hpp"

ShaderBuilderNodes::FunctionDefinition ShaderBuilderFunctionsLibrary::getFunctionCalculateSkinnedPosition(const ShaderBuilderNodes::Program& program, Ptr<const Material> material)
{
    using namespace ShaderBuilderNodes;
    Variable pos = Variable(GPUBuiltIn::Types::mVector4.mTypeName, "pos");
    FunctionDefinition func(GPUBuiltIn::Types::mVector4.mTypeName, "calculateSkinnedPosition", {pos});
    
    auto& bonesIDs = program.getAttribute(GPUBuiltIn::VertexInput::mBonesIDs.mName);
    auto& bonesWeights = program.getAttribute(GPUBuiltIn::VertexInput::mBonesWeights.mName);

    auto& MAX_BONES = program.getAttribute(GPUBuiltIn::Consts::MAX_BONES.mName);
    auto& MAX_BONE_INFLUENCE = program.getAttribute(GPUBuiltIn::Consts::MAX_BONE_INFLUENCE.mName);

    auto& hasAnimations = program.getAttribute(GPUBuiltIn::Uniforms::mIsAnimated.mName);
    auto& bonesTransform = program.getAttribute(GPUBuiltIn::Uniforms::mBonesTransform.mName);
    
    Variable finalPositon;
    Variable localPosition;
    
    func.body().
    variable(finalPositon, "vec4", "finalPositon", pos).
    ifBlock(hasAnimations).
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
        end().
    end().
    ret(finalPositon);

    return func;
}