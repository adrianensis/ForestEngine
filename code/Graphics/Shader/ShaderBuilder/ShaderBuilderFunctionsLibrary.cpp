#include "Graphics/Shader/ShaderBuilder/ShaderBuilderFunctionsLibrary.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Material/Material.hpp"

using namespace ShaderBuilderNodes;

void ShaderBuilderFunctionsLibrary::init(const ShaderBuilderNodes::Program& program, Ptr<const Material> material)
{
    if(material->getMaterialData().mIsSkinned)
    {
        registerFunctionCalculateSkinnedPosition(program, material);
    }
    
    if(material->getMaterialData().mReceiveLight)
    {
        registerFunctionCalculateDiffuse(program, material);
    }
}

void ShaderBuilderFunctionsLibrary::registerFunctionCalculateSkinnedPosition(const ShaderBuilderNodes::Program& program, Ptr<const Material> material)
{
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

void ShaderBuilderFunctionsLibrary::registerFunctionCalculateDiffuse(const ShaderBuilderNodes::Program& program, Ptr<const Material> material)
{
    FunctionDefinition func(GPUBuiltIn::Functions::mCalculateDiffuse);
    
    auto& inNormal = program.getAttribute(GPUBuiltIn::VertexOutput::mNormal.mName);
    auto& inPosition = program.getAttribute(GPUBuiltIn::VertexOutput::mFragPosition.mName);

    auto& ligthsBuffer = program.getSharedBuffer(GPUBuiltIn::SharedBuffers::mLights.mInstanceName);    
    Variable lights(ligthsBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));

    Variable norm;
    Variable lightDir;
    Variable lightPos = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[0]};
    Variable lightColor = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[1]};
    Variable lightStrength = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[2]};
    Variable ambient;

    func.body().
    variable(ambient, GPUBuiltIn::PrimitiveTypes::mFloat.mName, "ambient", "0.2");

    func.body().
    variable(norm, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "norm", call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {{"0.0"}, {"0.0"}, {"0.0"}}));

    if(material->getMaterialData().mUseModelMatrix && material->getMaterialData().mUseNormals)
    {
        func.body().
        set(norm, call("normalize", {inNormal}));
    }

    Variable diffuse;
    func.body().
    variable(lightDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "lightDir", call("normalize", {lights.at("0").dot(lightPos).sub(inPosition)})).
    variable(diffuse, GPUBuiltIn::PrimitiveTypes::mFloat.mName, "diffuse", call("max", {call("dot", {norm, lightDir}), {"0"}}));

    Variable diffuseColor;
    Variable diffusePlusAmbient;
    func.body().
    variable(diffusePlusAmbient, GPUBuiltIn::PrimitiveTypes::mFloat.mName, "diffusePlusAmbient", diffuse.add(ambient)).
    variable(diffuseColor, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "diffuseColor", lights.at("0").dot(lightColor).mul(diffusePlusAmbient)).
    set(diffuseColor.dot("a"), "1");

    // vec3 norm = normalize(Normal);
    // vec3 lightDir = normalize(light.position - FragPos);
    // float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse = light.diffuse * (diff * material.diffuse);

    func.body().
    ret(diffuseColor);

    mFunctions.insert_or_assign(func.mName, func);
}