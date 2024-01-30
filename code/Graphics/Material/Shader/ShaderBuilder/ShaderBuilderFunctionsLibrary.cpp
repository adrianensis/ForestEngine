#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilderFunctionsLibrary.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilderNodes.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Material/Material.hpp"

using namespace ShaderBuilderNodes;

void ShaderBuilderFunctionsLibrary::init(const ShaderBuilderNodes::Program& program, Ptr<const Material> material)
{
    if(material->getMaterialData().mIsSkinned)
    {
        registerFunctionCalculateBoneTransform(program, material);
    }
    
    if(material->getMaterialData().mReceiveLight)
    {
        registerFunctionCalculatePhong(program, material);
    }
}

void ShaderBuilderFunctionsLibrary::registerFunctionCalculateBoneTransform(const ShaderBuilderNodes::Program& program, Ptr<const Material> material)
{
    FunctionDefinition func(GPUBuiltIn::Functions::mCalculateBoneTransform);
    
    Variable finalBoneTransform;
    
    auto& bonesIDs = program.getAttribute(GPUBuiltIn::VertexInput::mBonesIDs.mName);
    auto& bonesWeights = program.getAttribute(GPUBuiltIn::VertexInput::mBonesWeights.mName);
    auto& MAX_BONES = program.getAttribute(GPUBuiltIn::Consts::MAX_BONES.mName);
    auto& MAX_BONE_INFLUENCE = program.getAttribute(GPUBuiltIn::Consts::MAX_BONE_INFLUENCE.mName);
    auto& bonesMatricesblock = program.getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices.mInstanceName);    
    Variable bonesTransform(bonesMatricesblock.mGPUSharedBufferData.getScopedGPUVariableData(0));

    Variable currentBoneTransform;
    Variable currentBoneTransformMulWeight;
    func.body().
    variable(finalBoneTransform, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "finalBoneTransform", call(GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, {{"0.0f"}})).
    forBlock("i", "<", MAX_BONE_INFLUENCE, "++").
        ifBlock(bonesIDs.at("i"), "==", {"-1"}).
            line("continue").
        end().
        ifBlock(bonesIDs.at("i"), ">=", MAX_BONES).
            line("break").
        end().
        variable(currentBoneTransform, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "currentBoneTransform", bonesTransform.at(bonesIDs.at("i"))).
        variable(currentBoneTransformMulWeight, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "currentBoneTransformMulWeight", currentBoneTransform.mul(bonesWeights.at("i"))).
        set(finalBoneTransform, finalBoneTransform.add(currentBoneTransformMulWeight)).
    end();
    
    func.body().
    ret(finalBoneTransform);

    mFunctions.insert_or_assign(func.mName, func);
}

void ShaderBuilderFunctionsLibrary::registerFunctionCalculatePhong(const ShaderBuilderNodes::Program& program, Ptr<const Material> material)
{
    FunctionDefinition func(GPUBuiltIn::Functions::mCalculatePhong);

    auto& globalDataBuffer = program.getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable cameraPosition(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(2));
    
    auto& inNormal = program.getAttribute(GPUBuiltIn::VertexOutput::mNormal.mName);
    auto& fragPosition = program.getAttribute(GPUBuiltIn::VertexOutput::mFragPosition.mName);

    auto& ligthsDataBuffer = program.getSharedBuffer(GPUBuiltIn::SharedBuffers::mLightsData.mInstanceName);    
    Variable lights(ligthsDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));

    Variable lightPos = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[0]};
    Variable lightColor = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[1]};
    Variable lightAmbientIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[2]};
    Variable lightSpecularIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[3]};

    Variable ambient;
    func.body().
    variable(ambient, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "ambient", lights.at("0").dot(lightAmbientIntensity).mul(lights.at("0").dot(lightColor)));

    Variable norm;
    func.body().
    variable(norm, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "norm", call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {{"0.0"}, {"0.0"}, {"0.0"}}));

    if(material->getMaterialData().mUseNormals)
    {
        func.body().
        set(norm, call("normalize", {inNormal}));
    }

    Variable lightDir;
    Variable diffuseValue;
    Variable diffuse;
    func.body().
    variable(lightDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "lightDir", call("normalize", {lights.at("0").dot(lightPos).sub(fragPosition)})).
    variable(diffuseValue, GPUBuiltIn::PrimitiveTypes::mFloat.mName, "diffuseValue", call("max", {call("dot", {norm, lightDir}), {"0"}})).
    variable(diffuse, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "diffuse", diffuseValue.mul(lights.at("0").dot(lightColor)));

    Variable viewDir;
    Variable reflectDir;
    Variable specularValue;
    Variable specular;
    func.body().
    variable(viewDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "viewDir", call("normalize", {cameraPosition.sub(fragPosition)})).
    variable(reflectDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "reflectDir", call("reflect", {viewDir.neg(), norm})).
    variable(specularValue, GPUBuiltIn::PrimitiveTypes::mFloat.mName, "specularValue", call("pow", {call("max", {call("dot", {viewDir, reflectDir}), {"0.0"}}), {"32"}})).
    variable(specular, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "specular", lights.at("0").dot(lightSpecularIntensity).mul(specularValue).mul(lights.at("0").dot(lightColor)));

    Variable phong;
    func.body().
    variable(phong, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "phong", ambient.add(diffuse).add(specular)).
    ret(call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {phong, {"1"}}));

    mFunctions.insert_or_assign(func.mName, func);
}