#include "Graphics/Material/MaterialRuntime/MaterialRuntimePBR.hpp"
#include "Graphics/Material/Material.hpp"
using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

// PBR METALLIC

std::vector<GPUStructDefinition::GPUStructVariable> MaterialRuntimePBR::generateMaterialPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {GPUBuiltIn::PrimitiveTypes::mVector4, "BaseColor"},
        {GPUBuiltIn::PrimitiveTypes::mVector3, "Metallic"},
        {GPUBuiltIn::PrimitiveTypes::mVector3, "Roughness"}
    };

    return propertiesBlock;
}

void MaterialRuntimePBR::createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    MaterialRuntimeDefault::createFragmentShader(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    if(mMaterial->getMaterialData().mReceiveLight)
    {
        fragmentShaderShadingModel(shaderBuilder);
    }
}

void MaterialRuntimePBR::fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const
{
    MaterialRuntimeDefault::fragmentShaderBaseColor(shaderBuilder);

    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor);
    Variable lightingModel = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    
    Variable baseColor = shaderBuilder.getVariableFromCache("baseColor");

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable instanceBaseColor = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    shaderBuilder.getMain().
    set(baseColor, propertiesBlock.at(materialInstanceId).dot(instanceBaseColor));

    shaderBuilder.getMain().
    set(outColor, baseColor);
}

void MaterialRuntimePBR::fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const
{
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor);

    Variable PBRMetallicRoughness;
    shaderBuilder.getMain().
    variable(PBRMetallicRoughness, GPUBuiltIn::PrimitiveTypes::mVector4, "PBRMetallicRoughness", call(mCalculatePBR, {})).
    set(outColor, outColor.add(PBRMetallicRoughness));
}

void MaterialRuntimePBR::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    MaterialRuntimeDefault::registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    
    if(mMaterial->getMaterialData().mReceiveLight)
    {
        registerFunctionCalculatePBR(shaderBuilder);
    }
}

// TOOD: Make actul PBRMetallicRoughness, I'm just emulating Phong with PBRMetallicRoughness parameters for now.
void MaterialRuntimePBR::registerFunctionCalculatePBR(ShaderBuilder& shaderBuilder) const
{
    FunctionDefinition func(mCalculatePBR);

    auto& globalDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable cameraPosition(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(2));
    auto& inNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mNormal);
    auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mFragPosition);
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
    Variable materialBaseColor = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    Variable materialMetallic = {getPropertiesBlockStructDefinition().mPrimitiveVariables[1]};
    Variable materialRoughness = {getPropertiesBlockStructDefinition().mPrimitiveVariables[2]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mMaterialInstanceID);

    Variable materialBaseColor3;
    func.body().
    variable(materialBaseColor3, GPUBuiltIn::PrimitiveTypes::mVector3, "materialBaseColor3", call(GPUBuiltIn::PrimitiveTypes::mVector3, {
        propertiesBlock.at(materialInstanceId).dot(materialBaseColor).dot("r"),
        propertiesBlock.at(materialInstanceId).dot(materialBaseColor).dot("g"),
        propertiesBlock.at(materialInstanceId).dot(materialBaseColor).dot("b")
    }));

    Variable ambient;
    func.body().
    variable(ambient, GPUBuiltIn::PrimitiveTypes::mVector3, "ambient", materialBaseColor3.mul(lights.at("0").dot(lightAmbientIntensity).mul(lights.at("0").dot(lightAmbient))));
    Variable norm;
    func.body().
    variable(norm, GPUBuiltIn::PrimitiveTypes::mVector3, "norm", call(GPUBuiltIn::PrimitiveTypes::mVector3, {{"0.0"}, {"0.0"}, {"0.0"}}));

    if(mMaterial->getMaterialData().mUseNormals)
    {
        func.body().
        set(norm, call("normalize", {inNormal}));
    }

    Variable lightDir;
    Variable diffuseValue;
    Variable diffuse;
    func.body().
    variable(lightDir, GPUBuiltIn::PrimitiveTypes::mVector3, "lightDir", call("normalize", {lights.at("0").dot(lightPos).sub(fragPosition)})).
    variable(diffuseValue, GPUBuiltIn::PrimitiveTypes::mFloat, "diffuseValue", call("max", {call("dot", {norm, lightDir}), {"0"}})).
    variable(diffuse, GPUBuiltIn::PrimitiveTypes::mVector3, "diffuse", materialBaseColor3.mul(lights.at("0").dot(lightSpecularIntensity).mul(diffuseValue.mul(lights.at("0").dot(lightDiffuse)))));
    Variable viewDir;
    Variable reflectDir;
    Variable specularValue;
    Variable specular;

    Variable roughness;
    Variable metallic;
    func.body().
    variable(roughness, GPUBuiltIn::PrimitiveTypes::mFloat, "roughness", propertiesBlock.at(materialInstanceId).dot(materialRoughness).dot("x")).
    variable(metallic, GPUBuiltIn::PrimitiveTypes::mFloat, "metallic", propertiesBlock.at(materialInstanceId).dot(materialMetallic).dot("x"));
    if(mMaterial->hasTexture(TextureMap::METALLIC_ROUGHNESS))
    {
        auto& samplerMetallicRoughness = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getSampler(std::string(EnumsManager::toString<TextureMap>(TextureMap::METALLIC_ROUGHNESS))).mName);
        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord);

        Variable metallicRoughnessPack;
        func.body().
        variable(metallicRoughnessPack, GPUBuiltIn::PrimitiveTypes::mVector4, "metallicRoughnessPack", call("texture2D", {samplerMetallicRoughness, inTextureCoord})).
        set(roughness, metallicRoughnessPack.dot("b")).
        set(metallic, metallicRoughnessPack.dot("g"));
    }

    func.body().
    variable(viewDir, GPUBuiltIn::PrimitiveTypes::mVector3, "viewDir", call("normalize", {cameraPosition.sub(fragPosition)})).
    variable(reflectDir, GPUBuiltIn::PrimitiveTypes::mVector3, "reflectDir", call("reflect", {viewDir.neg(), norm})).
    variable(specularValue, GPUBuiltIn::PrimitiveTypes::mFloat, "specularValue", call("pow", {call("max", {call("dot", {viewDir, reflectDir}), {"0.0"}}), roughness})).
    variable(specular, GPUBuiltIn::PrimitiveTypes::mVector3, "specular", metallic.mul(lights.at("0").dot(lightSpecularIntensity).mul(specularValue).mul(lights.at("0").dot(lightSpecular))));
    Variable PBRMetallicRoughness;
    func.body().
    variable(PBRMetallicRoughness, GPUBuiltIn::PrimitiveTypes::mVector3, "PBRMetallicRoughness", specular).
    // variable(PBRMetallicRoughness, GPUBuiltIn::PrimitiveTypes::mVector3, "PBRMetallicRoughness", ambient.add(diffuse).add(specular)).
    ret(call(GPUBuiltIn::PrimitiveTypes::mVector4, {PBRMetallicRoughness, {"1"}}));
    // ret(propertiesBlock.at(materialInstanceId).dot(materialBaseColor));

    shaderBuilder.get().function(func);
}
