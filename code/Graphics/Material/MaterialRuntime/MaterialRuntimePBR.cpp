#include "Graphics/Material/MaterialRuntime/MaterialRuntimePBR.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Light/Light.hpp"
using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

// PBR METALLIC

std::vector<GPUStructDefinition::GPUStructVariable> MaterialRuntimePBR::generateMaterialPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {GPUBuiltIn::PrimitiveTypes::mVector4, "BaseColor"},
        {GPUBuiltIn::PrimitiveTypes::mFloat, "Metallic"},
        {GPUBuiltIn::PrimitiveTypes::mFloat, "Roughness"}
    };

    return propertiesBlock;
}

void MaterialRuntimePBR::fragmentShaderCode(ShaderBuilder& shaderBuilder) const
{
    // MaterialRuntimeDefault::fragmentShaderCode(shaderBuilder);

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

    if(mMaterial->getMaterialData().mReceiveLight)
    {
        Variable PBRMetallicRoughness;
        shaderBuilder.getMain().
        variable(PBRMetallicRoughness, GPUBuiltIn::PrimitiveTypes::mVector4, "PBRMetallicRoughness", call(mCalculatePBR, {})).
        set(outColor, PBRMetallicRoughness);
    }
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

    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor);
    
    auto& globalDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable cameraPosition(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(2));
    auto& inNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mNormal);
    auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mFragPosition);
    auto& ligthsDataBuffer = shaderBuilder.get().getSharedBuffer(LightsBuiltIn::mLightsBufferData.mInstanceName);    
    Variable lights(ligthsDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
    Variable lightPos = {LightsBuiltIn::mLightStructDefinition.mPrimitiveVariables[0]};
    Variable lightAmbient = {LightsBuiltIn::mLightStructDefinition.mPrimitiveVariables[1]};
    Variable lightDiffuse = {LightsBuiltIn::mLightStructDefinition.mPrimitiveVariables[2]};
    Variable lightSpecular = {LightsBuiltIn::mLightStructDefinition.mPrimitiveVariables[3]};
    Variable lightAmbientIntensity = {LightsBuiltIn::mLightStructDefinition.mPrimitiveVariables[4]};
    Variable lightDiffuseIntensity = {LightsBuiltIn::mLightStructDefinition.mPrimitiveVariables[5]};
    Variable lightSpecularIntensity = {LightsBuiltIn::mLightStructDefinition.mPrimitiveVariables[6]};

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable materialBaseColor = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    Variable materialMetallic = {getPropertiesBlockStructDefinition().mPrimitiveVariables[1]};
    Variable materialRoughness = {getPropertiesBlockStructDefinition().mPrimitiveVariables[2]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mMaterialInstanceID);

    Variable roughness;
    Variable metallic;
    func.body().
    variable(roughness, GPUBuiltIn::PrimitiveTypes::mFloat, "roughness", propertiesBlock.at(materialInstanceId).dot(materialRoughness)).
    variable(metallic, GPUBuiltIn::PrimitiveTypes::mFloat, "metallic", propertiesBlock.at(materialInstanceId).dot(materialMetallic));
    if(mMaterial->hasTexture(TextureMap::METALLIC_ROUGHNESS))
    {
        auto& samplerMetallicRoughness = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getSampler(std::string(EnumsManager::toString<TextureMap>(TextureMap::METALLIC_ROUGHNESS))).mName);
        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mTextureCoord);

        Variable metallicRoughnessPack;
        func.body().
        variable(metallicRoughnessPack, GPUBuiltIn::PrimitiveTypes::mVector4, "metallicRoughnessPack", call("texture", {samplerMetallicRoughness, inTextureCoord})).
        set(roughness, metallicRoughnessPack.dot("g")).
        set(metallic, metallicRoughnessPack.dot("b"));
    }

    Variable albedo;
    func.body().
    variable(albedo, GPUBuiltIn::PrimitiveTypes::mVector3, "albedo", call(GPUBuiltIn::PrimitiveTypes::mVector3, {
        outColor.dot("r"),
        outColor.dot("g"),
        outColor.dot("b")
    }));

    /*
        vec3 N = normalize(Normal);
        vec3 V = normalize(camPos - WorldPos);

        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);
    */

    Variable N;
    func.body().
    variable(N, GPUBuiltIn::PrimitiveTypes::mVector3, "N", call(GPUBuiltIn::PrimitiveTypes::mVector3, {{"0.0"}, {"0.0"}, {"0.0"}}));

    if(inNormal.isValid())
    {
        func.body().
        set(N, call("normalize", {inNormal}));
    }

    Variable V;
    Variable F0;
    func.body().
    variable(V, GPUBuiltIn::PrimitiveTypes::mVector3, "V", call("normalize", {cameraPosition.sub(fragPosition)})).
    variable(F0, GPUBuiltIn::PrimitiveTypes::mVector3, "F0", call(GPUBuiltIn::PrimitiveTypes::mVector3, {"0.04"s})).
    set(F0, call("mix", {F0, albedo, metallic}));

    /*
        // reflectance equation
        vec3 Lo = vec3(0.0);
    */
    Variable Lo;
    func.body().
    variable(Lo, GPUBuiltIn::PrimitiveTypes::mVector3, "Lo", call(GPUBuiltIn::PrimitiveTypes::mVector3, {"0"s}));

    Variable L;
    Variable H;
    Variable distance;
    Variable attenuation;
    Variable radiance;

    Variable NDF;
    Variable G;
    Variable F;
    Variable numerator;
    Variable denominator;
    Variable specular;

    Variable kS;
    Variable kD;

    Variable NdotL;

    func.body().
    forBlock("i", "<", Variable("1"), "++").

    /*
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;
    */
    variable(L, GPUBuiltIn::PrimitiveTypes::mVector3, "L", call("normalize", {lights.at("i").dot(lightPos).sub(fragPosition)})).
    variable(H, GPUBuiltIn::PrimitiveTypes::mVector3, "H", call("normalize", {V.add(L)})).
    variable(distance, GPUBuiltIn::PrimitiveTypes::mFloat, "distance", call("length", {lights.at("i").dot(lightPos).sub(fragPosition)})).
    variable(attenuation, GPUBuiltIn::PrimitiveTypes::mFloat, "attenuation", Variable("1.0").div(paren(distance.mul(distance)))).
    variable(radiance, GPUBuiltIn::PrimitiveTypes::mVector3, "radiance", lights.at("i").dot(lightAmbient).mul(attenuation)).

    /*
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
    */
    variable(NDF, GPUBuiltIn::PrimitiveTypes::mFloat, "NDF", call(mDistributionGGX, {N, H, roughness})).
    variable(G, GPUBuiltIn::PrimitiveTypes::mFloat, "G", call(mGeometrySmith, {N, V, L, roughness})).
    variable(F, GPUBuiltIn::PrimitiveTypes::mVector3, "F", call(mFresnelSchlick, {call("clamp",{call("dot", {N, H}), "0.0"s, "1.0"s}), F0})).
    variable(numerator, GPUBuiltIn::PrimitiveTypes::mVector3, "numerator", NDF.mul(G).mul(F)).
    variable(denominator, GPUBuiltIn::PrimitiveTypes::mFloat, "denominator",
        Variable("4.0").
        mul(call("max", {call("dot", {N, V}), "0.0"s})).
        mul(call("max", {call("dot", {N, L}), "0.0"s})).
        add("0.0001"s)
    ).
    variable(specular, GPUBuiltIn::PrimitiveTypes::mVector3, "specular", numerator.div(denominator)).

    /*
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	
    */
    variable(kS, GPUBuiltIn::PrimitiveTypes::mVector3, "kS", F).
    variable(kD, GPUBuiltIn::PrimitiveTypes::mVector3, "kD", call(GPUBuiltIn::PrimitiveTypes::mVector3, {"1.0"s}).sub(kS)).
    set(kD, kD.mul(paren(Variable("1.0").sub(metallic)))).

    /*
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    */
    variable(NdotL, GPUBuiltIn::PrimitiveTypes::mFloat, "NdotL", call("max", {call("dot", {N, L}), "0.0"s})).
    set(Lo, Lo.add(paren(paren(kD.mul(albedo).div(GPUBuiltIn::Consts::mPI).add(specular)).mul(radiance).mul(NdotL)))).
    end();

    /*
        // ambient lighting (note that the next IBL tutorial will replace 
        // this ambient lighting with environment lighting).
        vec3 ambient = vec3(0.03) * albedo * ao;
    */
    Variable ambient;
    func.body().
    variable(ambient, GPUBuiltIn::PrimitiveTypes::mVector3, "ambient", call(GPUBuiltIn::PrimitiveTypes::mVector3, {"0.03"s}).mul(albedo)/*.mul(ao)*/);

    /*
        vec3 color = ambient + Lo;

        // HDR tonemapping
        color = color / (color + vec3(1.0));
        // gamma correct
        color = pow(color, vec3(1.0/2.2)); 
    */

    Variable PBRFinalColor;
    func.body().
    variable(PBRFinalColor, GPUBuiltIn::PrimitiveTypes::mVector3, "PBRFinalColor", ambient.add(Lo)).
    set(PBRFinalColor, PBRFinalColor.div(paren(PBRFinalColor.add(call(GPUBuiltIn::PrimitiveTypes::mVector3, {"1.0"s}))))).
    set(PBRFinalColor, call("pow", {PBRFinalColor, call(GPUBuiltIn::PrimitiveTypes::mVector3, {"1.0/2.2"s})})).
    ret(call(GPUBuiltIn::PrimitiveTypes::mVector4, {PBRFinalColor, {"1"}}));

    shaderBuilder.get().function(func);
}