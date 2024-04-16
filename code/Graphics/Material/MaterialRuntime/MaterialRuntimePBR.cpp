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
    // MaterialRuntimeDefault::fragmentShaderBaseColor(shaderBuilder);

    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor);
    Variable lightingModel = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable instanceBaseColor = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    
    Variable baseColor;
    shaderBuilder.getMain().
    variable(baseColor, GPUBuiltIn::PrimitiveTypes::mVector4, "baseColor", propertiesBlock.at(materialInstanceId).dot(instanceBaseColor));

    shaderBuilder.getMain().
    set(outColor, baseColor);

    if(mMaterial->hasTexture(TextureMap::BASE_COLOR))
    {
        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord);
        auto& sampler = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getSampler(std::string(EnumsManager::toString<TextureMap>(TextureMap::BASE_COLOR))));
        shaderBuilder.getMain().
        set(outColor, call("texture", {sampler, inTextureCoord}));
    }
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
        registerFunctionsPBRHelpers(shaderBuilder);
        registerFunctionCalculatePBR(shaderBuilder);
    }
}

void MaterialRuntimePBR::registerFunctionsPBRHelpers(ShaderBuilder& shaderBuilder) const
{
    {
        FunctionDefinition funcGetNormalFromMap(mGetNormalFromMap);

        // Easy trick to get tangent-normals to world-space to keep PBR code simplified.
        // Don't worry if you don't get what's going on; you generally want to do normal 
        // mapping the usual way for performance anyways; I do plan make a note of this 
        // technique somewhere later in the normal mapping tutorial.
        Variable tangentNormal;
        Variable Q1;
        Variable Q2;
        Variable st1;
        Variable st2;
        Variable N;
        Variable T;
        Variable B;
        Variable TBN;

        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mTextureCoord);
        auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mFragPosition);
        auto& inNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mNormal);
        
        Variable normalFromTexture;
        funcGetNormalFromMap.body().
        variable(normalFromTexture, GPUBuiltIn::PrimitiveTypes::mVector4, "normalFromTexture", call(GPUBuiltIn::PrimitiveTypes::mVector4, {{"0.0"}, {"0.0"}, {"0.0"}, {"0.0"}}));

        if(mMaterial->hasTexture(TextureMap::NORMAL))
        {
            auto& samplerNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getSampler(std::string(EnumsManager::toString<TextureMap>(TextureMap::NORMAL))).mName);
            funcGetNormalFromMap.body().
            set(normalFromTexture, call("texture", {samplerNormal, inTextureCoord}));
        }

        funcGetNormalFromMap.body().
        variable(tangentNormal, GPUBuiltIn::PrimitiveTypes::mVector3, "tangentNormal",
            normalFromTexture.dot("xyz").mul("2"s).sub("1"s)).
        variable(Q1, GPUBuiltIn::PrimitiveTypes::mVector3, "Q1", call("dFdx", {fragPosition})).
        variable(Q2, GPUBuiltIn::PrimitiveTypes::mVector3, "Q2", call("dFdy", {fragPosition})).
        variable(st1, GPUBuiltIn::PrimitiveTypes::mVector2, "st1", call("dFdx", {inTextureCoord})).
        variable(st2, GPUBuiltIn::PrimitiveTypes::mVector2, "st2", call("dFdy", {inTextureCoord})).
        variable(N, GPUBuiltIn::PrimitiveTypes::mVector3, "N", call("normalize", {inNormal})).
        variable(T, GPUBuiltIn::PrimitiveTypes::mVector3, "T", call("normalize", {Q1.mul(st2.dot("t")).sub(Q2.mul(st1.dot("t")))})).
        variable(B, GPUBuiltIn::PrimitiveTypes::mVector3, "B", call("normalize", {call("cross", {N, T})}).mul("-1"s)).
        variable(TBN, GPUBuiltIn::PrimitiveTypes::mMatrix3, "TBN", call("mat3", {T, B, N})).
        ret(call("normalize", {TBN.mul(tangentNormal)}));

        shaderBuilder.get().function(funcGetNormalFromMap);
    }

    {
        FunctionDefinition funcDistributionGGX(mDistributionGGX);
        Variable N = funcDistributionGGX.mParameters[0];
        Variable H = funcDistributionGGX.mParameters[1];
        Variable roughness = funcDistributionGGX.mParameters[2];

        Variable a;
        Variable a2;
        Variable NdotH;
        Variable NdotH2;
        Variable nom;
        Variable denom;

        funcDistributionGGX.body().
        variable(a, GPUBuiltIn::PrimitiveTypes::mFloat, "a", call("dFdx", {roughness.mul(roughness)})).
        variable(a2, GPUBuiltIn::PrimitiveTypes::mFloat, "a2", a.mul(a)).
        variable(NdotH, GPUBuiltIn::PrimitiveTypes::mFloat, "NdotH", call("max", {call("dot", {N, H}), {"0.0"}})).
        variable(NdotH2, GPUBuiltIn::PrimitiveTypes::mFloat, "NdotH2", NdotH.mul(NdotH)).
        variable(nom, GPUBuiltIn::PrimitiveTypes::mFloat, "nom", a2).
        variable(denom, GPUBuiltIn::PrimitiveTypes::mFloat, "denom", NdotH2.mul(paren(a2.sub("1"s)).add("1"s))).
        set(denom, Variable(GPUBuiltIn::Consts::mPI).mul(denom.mul(denom))).
        ret(nom.div(denom));

        shaderBuilder.get().function(funcDistributionGGX);
    }

    {
        FunctionDefinition funcGeometrySchlickGGX(mGeometrySchlickGGX);
        Variable NdotV = funcGeometrySchlickGGX.mParameters[0];
        Variable roughness = funcGeometrySchlickGGX.mParameters[1];

        Variable r;
        Variable k;
        Variable nom;
        Variable denom;

        funcGeometrySchlickGGX.body().
        variable(r, GPUBuiltIn::PrimitiveTypes::mFloat, "r", roughness.add("1.0"s)).
        variable(k, GPUBuiltIn::PrimitiveTypes::mFloat, "k", r.mul(r).div("8.0"s)).
        variable(nom, GPUBuiltIn::PrimitiveTypes::mFloat, "nom", NdotV).
        variable(denom, GPUBuiltIn::PrimitiveTypes::mFloat, "denom", NdotV.mul(paren(Variable("1.0").sub(k)).add(k))).
        ret(nom.div(denom));

        shaderBuilder.get().function(funcGeometrySchlickGGX);
    }

    {
        FunctionDefinition funcGeometrySmith(mGeometrySmith);
        Variable N = funcGeometrySmith.mParameters[0];
        Variable V = funcGeometrySmith.mParameters[1];
        Variable L = funcGeometrySmith.mParameters[2];
        Variable roughness = funcGeometrySmith.mParameters[3];

        Variable NdotV;
        Variable NdotL;
        Variable ggx2;
        Variable ggx1;

        funcGeometrySmith.body().
        variable(NdotV, GPUBuiltIn::PrimitiveTypes::mFloat, "NdotV", call("max", {call("dot", {N, V}), {"0.0"}})).
        variable(NdotL, GPUBuiltIn::PrimitiveTypes::mFloat, "NdotL", call("max", {call("dot", {N, L}), {"0.0"}})).
        variable(ggx2, GPUBuiltIn::PrimitiveTypes::mFloat, "ggx2", call("geometrySchlickGGX", {NdotV, roughness})).
        variable(ggx1, GPUBuiltIn::PrimitiveTypes::mFloat, "ggx1", call("geometrySchlickGGX", {NdotL, roughness})).
        ret(ggx1.mul(ggx2));

        shaderBuilder.get().function(funcGeometrySmith);
    }

    {
        FunctionDefinition funcFresnelSchlick(mFresnelSchlick);
        Variable cosTheta = funcFresnelSchlick.mParameters[0];
        Variable F0 = funcFresnelSchlick.mParameters[1];

        funcFresnelSchlick.body().
        ret(
            F0.add(
                paren(Variable("1.0").sub(F0)).
                mul(
                    call("pow", {
                        call("clamp", {Variable("1.0").sub(cosTheta), {"0.0"}, {"1.0"}}),
                        {"5.0"}
                    })
                )
            )
        );
        shaderBuilder.get().function(funcFresnelSchlick);
    }
}

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
        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mTextureCoord);

        Variable metallicRoughnessPack;
        func.body().
        variable(metallicRoughnessPack, GPUBuiltIn::PrimitiveTypes::mVector4, "metallicRoughnessPack", call("texture", {samplerMetallicRoughness, inTextureCoord})).
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