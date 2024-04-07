#include "Graphics/Material/MaterialRuntimeDefault.hpp"
#include "Graphics/Material/Material.hpp"

// PBR SPECULAR

std::vector<GPUStructDefinition::GPUStructVariable> MaterialRuntimePBRSpecularGlossiness::generateMaterialPropertiesBlock()
{
    GPUDataType materialLightingModelStructDataType =
    {
        mLightingModelStructDefinition.mName,
        mLightingModelStructDefinition.getTypeSizeInBytes(),
        GPUPrimitiveDataType::STRUCT
    };

    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {materialLightingModelStructDataType, "materialLighting"}
    };

    return propertiesBlock;
}

using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

void MaterialRuntimePBRSpecularGlossiness::createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    MaterialRuntime::createFragmentShader(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    if(mMaterial->getMaterialData().mReceiveLight)
    {
        fragmentShaderShadingModel(shaderBuilder);
    }
}

void MaterialRuntimePBRSpecularGlossiness::fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const
{
    MaterialRuntime::fragmentShaderBaseColor(shaderBuilder);

    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);
    Variable lightingModel = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    
    Variable baseColor = shaderBuilder.getVariableFromCache("baseColor");

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable instanceDiffuse = {mLightingModelStructDefinition.mPrimitiveVariables[0]};
    mainFunc.body().
    set(baseColor, call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(instanceDiffuse), {"1.0"}}));

    mainFunc.body().
    set(outColor, baseColor);
}

void MaterialRuntimePBRSpecularGlossiness::fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);

    Variable PBRSpecularGlossiness;
    mainFunc.body().
    variable(PBRSpecularGlossiness, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "PBRSpecularGlossiness", call(mCalculatePBRSpecularGlossiness.mName, {})).
    set(outColor, outColor.mul(PBRSpecularGlossiness));
}

void MaterialRuntimePBRSpecularGlossiness::generateShaderBuilderData(MaterialRuntime::ShaderBuilderData& shaderBuilderData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(mLightingModelStructDefinition);

    if(mMaterial->getMaterialData().mReceiveLight)
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mLightsData);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(GPUBuiltIn::StructDefinitions::mLight);
    }

    MaterialRuntime::generateShaderBuilderData(shaderBuilderData, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
}

void MaterialRuntimePBRSpecularGlossiness::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    MaterialRuntime::registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    
    if(mMaterial->getMaterialData().mReceiveLight)
    {
        registerFunctionCalculatePBRSpecularGlossiness(shaderBuilder);
    }
}

void MaterialRuntimePBRSpecularGlossiness::registerFunctionCalculatePBRSpecularGlossiness(ShaderBuilder& shaderBuilder) const
{
    FunctionDefinition func(mCalculatePBRSpecularGlossiness);

    auto& globalDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable cameraPosition(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(2));
    auto& inNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mNormal.mName);
    auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mFragPosition.mName);
    auto& ligthsDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mLightsData.mInstanceName);    
    Variable lights(ligthsDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
    Variable lightPos = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[0]};
    Variable lightAmbient = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[1]};
    Variable lightDiffuse = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[2]};
    Variable lightSpecular = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[3]};
    Variable lightAmbientIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[4]};
    Variable lightDiffuseIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[5]};
    Variable lightSpecularIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[6]};

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable lightingModel = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    Variable materialDiffuse = {mLightingModelStructDefinition.mPrimitiveVariables[0]};
    Variable materialSpecular = {mLightingModelStructDefinition.mPrimitiveVariables[1]};
    Variable materialGlossiness = {mLightingModelStructDefinition.mPrimitiveVariables[2]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mMaterialInstanceID.mName);

    Variable ambient;
    func.body().
    variable(ambient, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "ambient", propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialDiffuse).mul(lights.at("0").dot(lightAmbientIntensity).mul(lights.at("0").dot(lightAmbient))));
    Variable norm;
    func.body().
    variable(norm, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "norm", call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {{"0.0"}, {"0.0"}, {"0.0"}}));

    if(mMaterial->getMaterialData().mUseNormals)
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
    variable(diffuse, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "diffuse", propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialDiffuse).mul(lights.at("0").dot(lightSpecularIntensity).mul(diffuseValue.mul(lights.at("0").dot(lightDiffuse)))));
    Variable viewDir;
    Variable reflectDir;
    Variable specularValue;
    Variable specular;
    func.body().
    variable(viewDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "viewDir", call("normalize", {cameraPosition.sub(fragPosition)})).
    variable(reflectDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "reflectDir", call("reflect", {viewDir.neg(), norm})).
    variable(specularValue, GPUBuiltIn::PrimitiveTypes::mFloat.mName, "specularValue", call("pow", {call("max", {call("dot", {viewDir, reflectDir}), {"0.0"}}), propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialGlossiness).dot("x")})).
    variable(specular, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "specular", propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialSpecular).mul(lights.at("0").dot(lightSpecularIntensity).mul(specularValue).mul(lights.at("0").dot(lightSpecular))));
    Variable PBRSpecularGlossiness;
    func.body().
    variable(PBRSpecularGlossiness, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "PBRSpecularGlossiness", ambient.add(diffuse).add(specular)).
    ret(call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {PBRSpecularGlossiness, {"1"}}));

    shaderBuilder.get().function(func);
}

// PBR METALLIC

std::vector<GPUStructDefinition::GPUStructVariable> MaterialRuntimePBRMetallicRoughness::generateMaterialPropertiesBlock()
{
    GPUDataType materialLightingModelStructDataType =
    {
        mLightingModelStructDefinition.mName,
        mLightingModelStructDefinition.getTypeSizeInBytes(),
        GPUPrimitiveDataType::STRUCT
    };

    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {materialLightingModelStructDataType, "materialLighting"}
    };

    return propertiesBlock;
}

using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

void MaterialRuntimePBRMetallicRoughness::createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    MaterialRuntime::createFragmentShader(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    if(mMaterial->getMaterialData().mReceiveLight)
    {
        fragmentShaderShadingModel(shaderBuilder);
    }
}

void MaterialRuntimePBRMetallicRoughness::fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const
{
    MaterialRuntime::fragmentShaderBaseColor(shaderBuilder);

    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);
    Variable lightingModel = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    
    Variable baseColor = shaderBuilder.getVariableFromCache("baseColor");

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable instanceBaseColor = {mLightingModelStructDefinition.mPrimitiveVariables[0]};
    mainFunc.body().
    set(baseColor, propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(instanceBaseColor));

    mainFunc.body().
    set(outColor, baseColor);
}

void MaterialRuntimePBRMetallicRoughness::fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);

    Variable PBRMetallicRoughness;
    mainFunc.body().
    variable(PBRMetallicRoughness, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "PBRMetallicRoughness", call(mCalculatePBRMetallicRoughness.mName, {})).
    set(outColor, outColor.mul(PBRMetallicRoughness));
}

void MaterialRuntimePBRMetallicRoughness::generateShaderBuilderData(MaterialRuntime::ShaderBuilderData& shaderBuilderData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(mLightingModelStructDefinition);

    if(mMaterial->getMaterialData().mReceiveLight)
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mLightsData);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(GPUBuiltIn::StructDefinitions::mLight);
    }

    MaterialRuntime::generateShaderBuilderData(shaderBuilderData, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
}

void MaterialRuntimePBRMetallicRoughness::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    MaterialRuntime::registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    
    if(mMaterial->getMaterialData().mReceiveLight)
    {
        registerFunctionCalculatePBRSpecularGlossiness(shaderBuilder);
    }
}

// TOOD: Make actul PBRMetallicRoughness, I'm just emulating Phong with PBRMetallicRoughness parameters for now.
void MaterialRuntimePBRMetallicRoughness::registerFunctionCalculatePBRSpecularGlossiness(ShaderBuilder& shaderBuilder) const
{
    FunctionDefinition func(mCalculatePBRMetallicRoughness);

    auto& globalDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable cameraPosition(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(2));
    auto& inNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mNormal.mName);
    auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mFragPosition.mName);
    auto& ligthsDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mLightsData.mInstanceName);    
    Variable lights(ligthsDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
    Variable lightPos = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[0]};
    Variable lightAmbient = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[1]};
    Variable lightDiffuse = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[2]};
    Variable lightSpecular = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[3]};
    Variable lightAmbientIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[4]};
    Variable lightDiffuseIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[5]};
    Variable lightSpecularIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[6]};

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable lightingModel = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    Variable materialBaseColor = {mLightingModelStructDefinition.mPrimitiveVariables[0]};
    Variable materialSpecular = {mLightingModelStructDefinition.mPrimitiveVariables[1]};
    Variable materialGlossiness = {mLightingModelStructDefinition.mPrimitiveVariables[2]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mMaterialInstanceID.mName);

    Variable materialBaseColor3;
    func.body().
    variable(materialBaseColor3, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "materialBaseColor3", call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {
        propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialBaseColor).dot("r"),
        propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialBaseColor).dot("g"),
        propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialBaseColor).dot("b")
    }));

    Variable ambient;
    func.body().
    variable(ambient, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "ambient", materialBaseColor3.mul(lights.at("0").dot(lightAmbientIntensity).mul(lights.at("0").dot(lightAmbient))));
    Variable norm;
    func.body().
    variable(norm, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "norm", call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {{"0.0"}, {"0.0"}, {"0.0"}}));

    if(mMaterial->getMaterialData().mUseNormals)
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
    variable(diffuse, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "diffuse", materialBaseColor3.mul(lights.at("0").dot(lightSpecularIntensity).mul(diffuseValue.mul(lights.at("0").dot(lightDiffuse)))));
    Variable viewDir;
    Variable reflectDir;
    Variable specularValue;
    Variable specular;
    func.body().
    variable(viewDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "viewDir", call("normalize", {cameraPosition.sub(fragPosition)})).
    variable(reflectDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "reflectDir", call("reflect", {viewDir.neg(), norm})).
    variable(specularValue, GPUBuiltIn::PrimitiveTypes::mFloat.mName, "specularValue", call("pow", {call("max", {call("dot", {viewDir, reflectDir}), {"0.0"}}), propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialGlossiness).dot("x")})).
    variable(specular, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "specular", propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialSpecular).mul(lights.at("0").dot(lightSpecularIntensity).mul(specularValue).mul(lights.at("0").dot(lightSpecular))));
    Variable PBRMetallicRoughness;
    func.body().
    variable(PBRMetallicRoughness, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "PBRMetallicRoughness", ambient.add(diffuse).add(specular)).
    ret(call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {PBRMetallicRoughness, {"1"}}));
    // ret(propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialBaseColor));

    shaderBuilder.get().function(func);
}
