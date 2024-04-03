#include "Graphics/Material/MaterialRuntimeDefault.hpp"
#include "Graphics/Material/Material.hpp"

std::vector<GPUStructDefinition::GPUStructVariable> MaterialRuntimePhong::generateMaterialPropertiesBlock()
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

void MaterialRuntimePhong::fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const
{
    MaterialRuntime::fragmentShaderBaseColor(shaderBuilder);

    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);
    Variable lightingModel = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    
    Variable baseColor = shaderBuilder.getVariableFromCache("baseColor");

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable instanceDiffuse = {mLightingModelStructDefinition.mPrimitiveVariables[1]};
    mainFunc.body().
    set(baseColor, call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(instanceDiffuse), {"1.0"}}));

    mainFunc.body().
    set(outColor, baseColor);
}

void MaterialRuntimePhong::fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);

    Variable phong;
    mainFunc.body().
    variable(phong, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "phong", call(GPUBuiltIn::Functions::mCalculatePhong.mName, {})).
    set(outColor, outColor.mul(phong));
}

void MaterialRuntimePhong::generateShaderBuilderData(MaterialRuntime::ShaderBuilderData& shaderBuilderData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(mLightingModelStructDefinition);
    MaterialRuntime::generateShaderBuilderData(shaderBuilderData, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
}

void MaterialRuntimePhong::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    MaterialRuntime::registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    
    if(mMaterial->getMaterialData().mReceiveLight)
    {
        registerFunctionCalculatePhong(shaderBuilder);
    }
}

void MaterialRuntimePhong::registerFunctionCalculatePhong(ShaderBuilder& shaderBuilder) const
{
    FunctionDefinition func(GPUBuiltIn::Functions::mCalculatePhong);

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
    Variable materialAmbient = {mLightingModelStructDefinition.mPrimitiveVariables[0]};
    Variable materialDiffuse = {mLightingModelStructDefinition.mPrimitiveVariables[1]};
    Variable materialSpecular = {mLightingModelStructDefinition.mPrimitiveVariables[2]};
    Variable materialShininess = {mLightingModelStructDefinition.mPrimitiveVariables[3]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mMaterialInstanceID.mName);
    // propertiesBlock.at(materialInstanceId).dot(depth)

    Variable ambient;
    func.body().
    variable(ambient, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "ambient", propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialAmbient).mul(lights.at("0").dot(lightAmbientIntensity).mul(lights.at("0").dot(lightAmbient))));
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
    variable(specularValue, GPUBuiltIn::PrimitiveTypes::mFloat.mName, "specularValue", call("pow", {call("max", {call("dot", {viewDir, reflectDir}), {"0.0"}}), propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialShininess).dot("x")})).
    variable(specular, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "specular", propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialSpecular).mul(lights.at("0").dot(lightSpecularIntensity).mul(specularValue).mul(lights.at("0").dot(lightSpecular))));
    Variable phong;
    func.body().
    variable(phong, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "phong", ambient.add(diffuse).add(specular)).
    ret(call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {phong, {"1"}}));

    shaderBuilder.get().function(func);
}
