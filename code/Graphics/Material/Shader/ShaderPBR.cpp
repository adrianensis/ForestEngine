#include "Graphics/Material/Shader/ShaderPBR.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Light/Light.hpp"
using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

// PBR METALLIC

std::vector<GPUStructDefinition::GPUStructVariable> ShaderPBR::generateMaterialPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {GPUBuiltIn::PrimitiveTypes::mVector4, "BaseColor"},
        {GPUBuiltIn::PrimitiveTypes::mFloat, "Metallic"},
        {GPUBuiltIn::PrimitiveTypes::mFloat, "Roughness"}
    };

    return propertiesBlock;
}

void ShaderPBR::vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const
{
    ShaderDefault::vertexShaderCalculatePositionOutput(shaderBuilder);

    if(getShaderData().mMaterial->getMaterialData().mReceiveShadows)
    {
        auto& shadowMappingBuffer = shaderBuilder.get().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData.mInstanceName);    
        Variable lightProjectionViewMatrix(shadowMappingBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));

        auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mFragPosition);
        auto& fragPositionLight = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mFragPositionLight);
        shaderBuilder.getMain().set(fragPositionLight, lightProjectionViewMatrix.mul(call(GPUBuiltIn::PrimitiveTypes::mVector4, {fragPosition, {"1"}})));
    }
}

void ShaderPBR::fragmentShaderCode(ShaderBuilder& shaderBuilder) const
{
    // ShaderDefault::fragmentShaderCode(shaderBuilder);

    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor);
    Variable lightingModel = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
    Variable propertiesBlock(getShaderData().mPropertiesBlockSharedBufferData.getScopedGPUVariableData(0));
    Variable instanceBaseColor = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
    
    Variable baseColor;
    shaderBuilder.getMain().
    variable(baseColor, GPUBuiltIn::PrimitiveTypes::mVector4, "baseColor", propertiesBlock.at(materialInstanceId).dot(instanceBaseColor));

    shaderBuilder.getMain().
    set(outColor, baseColor);

    if(hasTexture(TextureBindingNamesPBR::smBaseColor))
    {
        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoords.at(0));
        auto& textureHandler = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getTextureHandler(TextureBindingNamesPBR::smBaseColor));
        auto& texturesBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mTextures.mInstanceName);    
        Variable textures(texturesBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
        shaderBuilder.getMain().
        set(outColor, call("texture", {textures.at(textureHandler), inTextureCoord}));
    }

    if(getShaderData().mMaterial->getMaterialData().mReceiveLight)
    {
        Variable PBRMetallicRoughness;
        shaderBuilder.getMain().
        variable(PBRMetallicRoughness, GPUBuiltIn::PrimitiveTypes::mVector4, "PBRMetallicRoughness", call(mCalculatePBR, {call(GPUBuiltIn::PrimitiveTypes::mVector3, {outColor.dot("xyz")})})).
        set(outColor, PBRMetallicRoughness);
    }
}

void ShaderPBR::generateShaderBuilderData(ShaderDefault::ShaderBuilderData& shaderBuilderData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
    ShaderDefault::generateShaderBuilderData(shaderBuilderData, gpuVertexBuffersContainer);
    
    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(LightBuiltIn::mLightsBufferData);
    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mDirectionalLightStructDefinition);
    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mPointLightStructDefinition);
    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mSpotLightStructDefinition);

    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(LightBuiltIn::mShadowMappingBufferData);
}

void ShaderPBR::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
    ShaderDefault::registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer);

    registerFunctionsGetNormalFromMap(shaderBuilder);
    
    if(getShaderData().mMaterial->getMaterialData().mReceiveShadows && getShaderData().mFramebufferBindings.contains(TextureBindingNamesPBR::smShadowMap))
    {
        registerFunctionsShadowCalculation(shaderBuilder);
    }
    
    if(getShaderData().mMaterial->getMaterialData().mReceiveLight)
    {
        registerFunctionsPBRHelpers(shaderBuilder);
        registerFunctionCalculatePBR(shaderBuilder);
    }
}

void ShaderPBR::registerFunctionsGetNormalFromMap(ShaderBuilder& shaderBuilder) const
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

        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mTextureCoords.at(0));
        auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mFragPosition);
        auto& inNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mNormal);
        
        Variable normalFromTexture;
        funcGetNormalFromMap.body().
        variable(normalFromTexture, GPUBuiltIn::PrimitiveTypes::mVector4, "normalFromTexture", call(GPUBuiltIn::PrimitiveTypes::mVector4, {{"0.0"}, {"0.0"}, {"0.0"}, {"0.0"}}));

        if(hasTexture(TextureBindingNamesPBR::smNormal))
        {
            auto& textureHandler = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getTextureHandler(TextureBindingNamesPBR::smNormal).mName);
            auto& texturesBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mTextures.mInstanceName);    
            Variable textures(texturesBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
            funcGetNormalFromMap.body().
            set(normalFromTexture, call("texture", {textures.at(textureHandler), inTextureCoord}));
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
}

void ShaderPBR::registerFunctionsShadowCalculation(ShaderBuilder& shaderBuilder) const
{
    {
        FunctionDefinition funcCalculateShadow(mCalculateShadow);
        Variable fragPosLightSpace = funcCalculateShadow.mParameters[0];
        Variable lightDirection = funcCalculateShadow.mParameters[1];

        Variable projCoords;
        Variable bias;
        Variable normal;
        Variable lightDir;
        // Variable lightPos;
        Variable closestDepth;
        Variable currentDepth;
        Variable shadow;

        auto& samplerShadowMap = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getSampler(TextureBindingNamesPBR::smShadowMap).mName);
        auto& inNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mNormal);
        auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mFragPosition);

        funcCalculateShadow.body().
        variable(normal, GPUBuiltIn::PrimitiveTypes::mVector3, "normal", call(GPUBuiltIn::PrimitiveTypes::mVector3, {{"0.0"}, {"0.0"}, {"0.0"}}));

        if(hasTexture(TextureBindingNamesPBR::smNormal))
        {
            funcCalculateShadow.body().
            set(normal, call(mGetNormalFromMap, {}));
        }
        else if(inNormal.isValid())
        {
            funcCalculateShadow.body().
            set(normal, inNormal);
        }

        funcCalculateShadow.body().
        set(normal, call("normalize", {normal}));

        funcCalculateShadow.body().
        variable(projCoords, GPUBuiltIn::PrimitiveTypes::mVector3, "projCoords", fragPosLightSpace.dot("xyz").div(fragPosLightSpace.dot("w"))).
        set(projCoords, projCoords.mul("0.5"s).add("0.5"s)).
        variable(closestDepth, GPUBuiltIn::PrimitiveTypes::mFloat, "closestDepth", call("texture", {samplerShadowMap, projCoords.dot("xy")}).dot("r")).
        variable(lightDir, GPUBuiltIn::PrimitiveTypes::mVector3, "lightDir", call("normalize", {lightDirection})).
        // variable(lightPos, GPUBuiltIn::PrimitiveTypes::mVector3, "lightPos", call(GPUBuiltIn::PrimitiveTypes::mVector3, {"0"s,"0"s,"-100"s})).
        // variable(lightDir, GPUBuiltIn::PrimitiveTypes::mVector3, "lightDir", call("normalize", {lightPos.sub(fragPosition)})).
        variable(bias, GPUBuiltIn::PrimitiveTypes::mFloat, "bias", call("max", {Variable("0.0025").mul(paren(Variable("1.0").sub(call("dot", {normal, lightDir})))), {"0.0005"}})).
        // set(bias, call("clamp", {bias, "0.005"s, "0.01"s})).
        // set(bias, "0.0005"s).
        variable(currentDepth, GPUBuiltIn::PrimitiveTypes::mFloat, "currentDepth", projCoords.dot("z").sub(bias)).
        variable(shadow, GPUBuiltIn::PrimitiveTypes::mFloat, "shadow", currentDepth.great(closestDepth).ternary("0.5"s, "0.0"s)).
        ret(shadow);

        shaderBuilder.get().function(funcCalculateShadow);
    }
}

void ShaderPBR::registerFunctionsPBRHelpers(ShaderBuilder& shaderBuilder) const
{
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

void ShaderPBR::registerFunctionCalculatePBR(ShaderBuilder& shaderBuilder) const
{
    {
        FunctionDefinition funcCalculatePBRSingleLight(mCalculatePBRSingleLight);

        Variable albedo = funcCalculatePBRSingleLight.mParameters[0];
        Variable metallic = funcCalculatePBRSingleLight.mParameters[1];
        Variable roughness = funcCalculatePBRSingleLight.mParameters[2];
        Variable V = funcCalculatePBRSingleLight.mParameters[3];
        Variable N = funcCalculatePBRSingleLight.mParameters[4];
        Variable F0 = funcCalculatePBRSingleLight.mParameters[5];
        Variable lightDirection = funcCalculatePBRSingleLight.mParameters[6];
        Variable lightColor = funcCalculatePBRSingleLight.mParameters[7];

        Variable Lo;

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

        /*
            // calculate per-light radiance
            vec3 L = normalize(lightPositions[i] - WorldPos);
            vec3 H = normalize(V + L);
            float distance = length(lightPositions[i] - WorldPos);
            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = lightColors[i] * attenuation;
        */
        funcCalculatePBRSingleLight.body().
        variable(L, GPUBuiltIn::PrimitiveTypes::mVector3, "L", call("normalize", {lightDirection})).
        variable(H, GPUBuiltIn::PrimitiveTypes::mVector3, "H", call("normalize", {V.add(L)})).
        variable(distance, GPUBuiltIn::PrimitiveTypes::mFloat, "distance", call("length", {lightDirection})).
        variable(attenuation, GPUBuiltIn::PrimitiveTypes::mFloat, "attenuation", Variable("1.0").div(paren(distance.mul(distance)))).
        variable(radiance, GPUBuiltIn::PrimitiveTypes::mVector3, "radiance", lightColor.mul(attenuation)).
        
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
        variable(F, GPUBuiltIn::PrimitiveTypes::mVector3, "F", call(mFresnelSchlick, {call("max",{call("dot", {V, H}), "0.0"s}), F0})).
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
        variable(Lo, GPUBuiltIn::PrimitiveTypes::mVector3, "Lo", paren(paren(kD.mul(albedo).div(GPUBuiltIn::Consts::mPI).add(specular)).mul(radiance).mul(NdotL))).
        ret(Lo);

        shaderBuilder.get().function(funcCalculatePBRSingleLight);
    }

    {
        FunctionDefinition funcCalculatePBR(mCalculatePBR);
        Variable baseColor = funcCalculatePBR.mParameters[0];

        auto& globalDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
        Variable cameraPosition(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(1));
        auto& inNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mNormal);
        auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mFragPosition);
        auto& ligthsDataBuffer = shaderBuilder.get().getSharedBuffer(LightBuiltIn::mLightsBufferData.mInstanceName);    
        Variable pointLights(ligthsDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
        Variable pointLightPos = {LightBuiltIn::mPointLightStructDefinition.mPrimitiveVariables[0]};
        Variable pointLightDiffuse = {LightBuiltIn::mPointLightStructDefinition.mPrimitiveVariables[1]};

        Variable spotLights(ligthsDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(1));
        Variable spotLightPos = {LightBuiltIn::mSpotLightStructDefinition.mPrimitiveVariables[0]};
        Variable spotLightDiffuse = {LightBuiltIn::mSpotLightStructDefinition.mPrimitiveVariables[1]};
        Variable spotLightInnerCutOff = {LightBuiltIn::mSpotLightStructDefinition.mPrimitiveVariables[2]};
        Variable spotLightOuterCutOff = {LightBuiltIn::mSpotLightStructDefinition.mPrimitiveVariables[3]};

        Variable directionalLight(ligthsDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(2));
        Variable directionalLightDirection = {LightBuiltIn::mDirectionalLightStructDefinition.mPrimitiveVariables[0]};
        Variable directionalLightDiffuse = {LightBuiltIn::mDirectionalLightStructDefinition.mPrimitiveVariables[1]};

        Variable propertiesBlock(getShaderData().mPropertiesBlockSharedBufferData.getScopedGPUVariableData(0));
        Variable materialBaseColor = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
        Variable materialMetallic = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[1]};
        Variable materialRoughness = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[2]};
        auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mMaterialInstanceID);

        Variable roughness;
        Variable metallic;
        funcCalculatePBR.body().
        variable(roughness, GPUBuiltIn::PrimitiveTypes::mFloat, "roughness", propertiesBlock.at(materialInstanceId).dot(materialRoughness)).
        variable(metallic, GPUBuiltIn::PrimitiveTypes::mFloat, "metallic", propertiesBlock.at(materialInstanceId).dot(materialMetallic));
        if(hasTexture(TextureBindingNamesPBR::smMetallicRoughness))
        {
            auto& textureHandler = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getTextureHandler(TextureBindingNamesPBR::smMetallicRoughness).mName);
            auto& texturesBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mTextures.mInstanceName);    
            Variable textures(texturesBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
            auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mTextureCoords.at(0));

            Variable metallicRoughnessPack;
            funcCalculatePBR.body().
            variable(metallicRoughnessPack, GPUBuiltIn::PrimitiveTypes::mVector4, "metallicRoughnessPack", call("texture", {textures.at(textureHandler), inTextureCoord})).
            set(roughness, metallicRoughnessPack.dot("g")).
            set(metallic, metallicRoughnessPack.dot("b"));
        }

        // base color gamma correct
        Variable albedo;
        funcCalculatePBR.body().
        variable(albedo, GPUBuiltIn::PrimitiveTypes::mVector3, "albedo", call("pow", {baseColor, "vec3(2.2)"s}));

        /*
            vec3 N = normalize(Normal);
            vec3 V = normalize(camPos - WorldPos);

            // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
            // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
            vec3 F0 = vec3(0.04); 
            F0 = mix(F0, albedo, metallic);
        */

        Variable N;
        funcCalculatePBR.body().
        variable(N, GPUBuiltIn::PrimitiveTypes::mVector3, "N", call(GPUBuiltIn::PrimitiveTypes::mVector3, {{"0.0"}, {"0.0"}, {"0.0"}}));

        if(hasTexture(TextureBindingNamesPBR::smNormal))
        {
            funcCalculatePBR.body().
            set(N, call(mGetNormalFromMap, {}));
        }
        else if(inNormal.isValid())
        {
            funcCalculatePBR.body().
            set(N, inNormal);
        }

        funcCalculatePBR.body().
        set(N, call("normalize", {N}));

        Variable V;
        Variable F0;
        funcCalculatePBR.body().
        variable(V, GPUBuiltIn::PrimitiveTypes::mVector3, "V", call("normalize", {cameraPosition.sub(fragPosition)})).
        variable(F0, GPUBuiltIn::PrimitiveTypes::mVector3, "F0", call(GPUBuiltIn::PrimitiveTypes::mVector3, {"0.04"s})).
        set(F0, call("mix", {F0, albedo, metallic}));

        /*
            // reflectance equation
            vec3 Lo = vec3(0.0);
        */
        Variable Lo;
        funcCalculatePBR.body().
        variable(Lo, GPUBuiltIn::PrimitiveTypes::mVector3, "Lo", call(GPUBuiltIn::PrimitiveTypes::mVector3, {"0"s}));


        // funcCalculatePBR.body().
        // forBlock("i", "<", Variable("5"), "++").
        //     set(Lo, Lo.add(call(mCalculatePBRSingleLight, 
        //         {
        //             albedo, metallic, roughness, V, N, F0,
        //             pointLights.at("i").dot(pointLightPos).sub(fragPosition),
        //             pointLights.at("i").dot(pointLightDiffuse)
        //         }))).
        // end();

        Variable lightDirection;
        Variable lightDiffuse;
        funcCalculatePBR.body().
        variable(lightDirection, GPUBuiltIn::PrimitiveTypes::mVector3, "lightDirection", directionalLight.dot(directionalLightDirection)).
        variable(lightDiffuse, GPUBuiltIn::PrimitiveTypes::mVector3, "lightDiffuse", directionalLight.dot(directionalLightDiffuse));
        
        funcCalculatePBR.body().
        set(Lo, Lo.add(call(mCalculatePBRSingleLight, 
        {
            albedo, metallic, roughness, V, N, F0,
            lightDirection,
            lightDiffuse
        })));

        /*
            // ambient lighting (note that the next IBL tutorial will replace 
            // this ambient lighting with environment lighting).
            vec3 ambient = vec3(0.03) * albedo * ao;
        */

        Variable ambient;
        funcCalculatePBR.body().
        variable(ambient, GPUBuiltIn::PrimitiveTypes::mVector3, "ambient", call(GPUBuiltIn::PrimitiveTypes::mVector3, {"0.03"s}).mul(albedo)/*.mul(ao)*/);

        /*
            vec3 color = ambient + Lo;

            // HDR tonemapping
            color = color / (color + vec3(1.0));
            // gamma correct
            color = pow(color, vec3(1.0/2.2)); 
        */

        Variable PBRFinalColor;
        funcCalculatePBR.body().
        variable(PBRFinalColor, GPUBuiltIn::PrimitiveTypes::mVector3, "PBRFinalColor", ambient.add(Lo)).
        set(PBRFinalColor, PBRFinalColor.div(paren(PBRFinalColor.add(call(GPUBuiltIn::PrimitiveTypes::mVector3, {"1.0"s}))))).
        set(PBRFinalColor, call("pow", {PBRFinalColor, call(GPUBuiltIn::PrimitiveTypes::mVector3, {"1.0/2.2"s})}));

        Variable sampler = GPUBuiltIn::Uniforms::getSampler(TextureBindingNamesPBR::smShadowMap);
        if(getShaderData().mMaterial->getMaterialData().mReceiveShadows && getShaderData().mFramebufferBindings.contains(TextureBindingNamesPBR::smShadowMap))
        {
            auto& fragPositionLight = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mFragPositionLight);
            
            Variable shadow;
            funcCalculatePBR.body().
            variable(shadow, GPUBuiltIn::PrimitiveTypes::mFloat, "shadow", call(mCalculateShadow, {fragPositionLight, lightDirection})).
            set(PBRFinalColor, PBRFinalColor.mul(paren(Variable("1.0").sub(shadow))));
        }

        funcCalculatePBR.body().
        ret(call(GPUBuiltIn::PrimitiveTypes::mVector4, {PBRFinalColor, {"1"}}));

        shaderBuilder.get().function(funcCalculatePBR);
    }
}