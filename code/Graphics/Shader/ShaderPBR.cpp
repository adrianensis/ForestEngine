#include "Graphics/Shader/ShaderPBR.hpp"
#include "Graphics/Light/Light.hpp"
using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

// PBR METALLIC

std::vector<GPUStructDefinition::GPUStructVariable> ShaderPBR::generateShaderPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {GPUShaderDefinitions::PrimitiveTypes::mVector4, "BaseColor"},
        {GPUShaderDefinitions::PrimitiveTypes::mFloat, "Metallic"},
        {GPUShaderDefinitions::PrimitiveTypes::mFloat, "Roughness"}
    };

    return propertiesBlock;
}

void ShaderPBR::registerTextures()
{
    ShaderDefault::registerTextures();
    mTextures.insert(TextureBindingNamesPBR::smMetallicRoughness);
    mTextures.insert(TextureBindingNamesPBR::smNormal);
    mTextures.insert(TextureBindingNamesPBR::smShadowMap);
}

void ShaderPBR::vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const
{
    ShaderDefault::vertexShaderCalculatePositionOutput(shaderBuilder);

    auto& shadowMappingBuffer = shaderBuilder.get().getUniformBuffer(LightBuiltIn::mShadowMappingBufferData.mInstanceName);    
    Variable lightProjectionViewMatrix(shadowMappingBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));

    auto& fragPosition = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mFragPosition);
    auto& fragPositionLight = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mFragPositionLight);
    shaderBuilder.getMain().set(fragPositionLight, lightProjectionViewMatrix.mul(call(GPUShaderDefinitions::PrimitiveTypes::mVector4, {fragPosition, {"1"}})));
}

void ShaderPBR::fragmentShaderCode(ShaderBuilder& shaderBuilder) const
{
    // ShaderDefault::fragmentShaderCode(shaderBuilder);

    auto& shaderInstanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mShaderInstanceID);
    auto& outColor = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentOutput::mColor);
    Variable lightingModel = {mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
    Variable propertiesBlock(mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
    Variable instanceBaseColor = {mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
    
    Variable baseColor;
    shaderBuilder.getMain().
    variable(baseColor, GPUShaderDefinitions::PrimitiveTypes::mVector4, "baseColor", propertiesBlock.at(shaderInstanceId).dot(instanceBaseColor));

    shaderBuilder.getMain().
    set(outColor, baseColor);

    auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mTextureCoords.at(0));
    auto& textureHandler = shaderBuilder.get().getAttribute(GPUShaderDefinitions::Uniforms::getTextureHandler(TextureBindingNamesPBR::smBaseColor));
    auto& texturesBuffer = shaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mTextures.mInstanceName);    
    Variable textures(texturesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
    if(inTextureCoord.isValid())
    {
        shaderBuilder.getMain().
        // ifBlock(textureHandler.notEq("0"s)).
            set(outColor, call("texture", {/*textures.at(textureHandler)*/textureHandler, inTextureCoord}));
        // end();
    }

    Variable PBRMetallicRoughness;
    shaderBuilder.getMain().
    variable(PBRMetallicRoughness, GPUShaderDefinitions::PrimitiveTypes::mVector4, "PBRMetallicRoughness", call(mCalculatePBR, {call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {outColor.dot("xyz")})})).
    set(outColor, PBRMetallicRoughness);
}

void ShaderPBR::generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
    ShaderDefault::generateShaderGenerationData(shaderGenerationData, gpuVertexBuffersContainer);
    
    shaderGenerationData.mCommonVariables.mUniformBuffers.push_back(LightBuiltIn::mLightsBufferData);
    shaderGenerationData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mDirectionalLightStructDefinition);
    shaderGenerationData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mPointLightStructDefinition);
    shaderGenerationData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mSpotLightStructDefinition);

    shaderGenerationData.mCommonVariables.mUniformBuffers.push_back(LightBuiltIn::mShadowMappingBufferData);
}

void ShaderPBR::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const
{
    ShaderDefault::registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuShaderDescriptorSets);

    registerFunctionsGetNormalFromMap(shaderBuilder);
    
    if(mFramebufferBindings.contains(TextureBindingNamesPBR::smShadowMap))
    {
        registerFunctionsShadowCalculation(shaderBuilder);
    }
    
    registerFunctionsPBRHelpers(shaderBuilder);
    registerFunctionCalculatePBR(shaderBuilder);
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

        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mTextureCoords.at(0));
        auto& fragPosition = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mFragPosition);
        auto& inNormal = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mNormal);
        
        if(inTextureCoord.isValid())
        {
            Variable normalFromTexture;
            funcGetNormalFromMap.body().
            variable(normalFromTexture, GPUShaderDefinitions::PrimitiveTypes::mVector4, "normalFromTexture", call(GPUShaderDefinitions::PrimitiveTypes::mVector4, {{"0.0"}, {"0.0"}, {"0.0"}, {"0.0"}}));

            auto& textureHandler = shaderBuilder.get().getAttribute(GPUShaderDefinitions::Uniforms::getTextureHandler(TextureBindingNamesPBR::smNormal).mName);
            auto& texturesBuffer = shaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mTextures.mInstanceName);    
            Variable textures(texturesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
            funcGetNormalFromMap.body().
            // ifBlock(textureHandler.notEq("0"s)).
                set(normalFromTexture, call("texture", {/*textures.at(textureHandler)*/textureHandler, inTextureCoord}));
            // end();

            funcGetNormalFromMap.body().
            variable(tangentNormal, GPUShaderDefinitions::PrimitiveTypes::mVector3, "tangentNormal",
                normalFromTexture.dot("xyz").mul("2"s).sub("1"s)).
            variable(Q1, GPUShaderDefinitions::PrimitiveTypes::mVector3, "Q1", call("dFdx", {fragPosition})).
            variable(Q2, GPUShaderDefinitions::PrimitiveTypes::mVector3, "Q2", call("dFdy", {fragPosition})).
            variable(st1, GPUShaderDefinitions::PrimitiveTypes::mVector2, "st1", call("dFdx", {inTextureCoord})).
            variable(st2, GPUShaderDefinitions::PrimitiveTypes::mVector2, "st2", call("dFdy", {inTextureCoord})).
            variable(N, GPUShaderDefinitions::PrimitiveTypes::mVector3, "N", call("normalize", {inNormal})).
            variable(T, GPUShaderDefinitions::PrimitiveTypes::mVector3, "T", call("normalize", {Q1.mul(st2.dot("t")).sub(Q2.mul(st1.dot("t")))})).
            variable(B, GPUShaderDefinitions::PrimitiveTypes::mVector3, "B", call("normalize", {call("cross", {N, T})}).mul("-1"s)).
            variable(TBN, GPUShaderDefinitions::PrimitiveTypes::mMatrix3, "TBN", call("mat3", {T, B, N})).
            ret(call("normalize", {TBN.mul(tangentNormal)}));
        }
        else
        {
            funcGetNormalFromMap.body().
            ret(call("normalize", {inNormal}));
        }

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

        auto& samplerShadowMap = shaderBuilder.get().getAttribute(GPUShaderDefinitions::Uniforms::getSampler(TextureBindingNamesPBR::smShadowMap).mName);
        auto& inNormal = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mNormal);
        auto& fragPosition = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mFragPosition);

        funcCalculateShadow.body().
        variable(normal, GPUShaderDefinitions::PrimitiveTypes::mVector3, "normal", call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {{"0.0"}, {"0.0"}, {"0.0"}}));

            auto& textureHandler = shaderBuilder.get().getAttribute(GPUShaderDefinitions::Uniforms::getTextureHandler(TextureBindingNamesPBR::smNormal));
            funcCalculateShadow.body().
            // ifBlock(textureHandler.notEq("0"s)).
                // set(normal, call(mGetNormalFromMap, {})).
            // end().
            // elseBlock().
                set(normal, inNormal);
            // end();

        funcCalculateShadow.body().
        set(normal, call("normalize", {normal}));

        funcCalculateShadow.body().
        variable(projCoords, GPUShaderDefinitions::PrimitiveTypes::mVector3, "projCoords", fragPosLightSpace.dot("xyz").div(fragPosLightSpace.dot("w"))).
        set(projCoords, projCoords.mul("0.5"s).add("0.5"s)).
        variable(closestDepth, GPUShaderDefinitions::PrimitiveTypes::mFloat, "closestDepth", call("texture", {samplerShadowMap, projCoords.dot("xy")}).dot("r")).
        variable(lightDir, GPUShaderDefinitions::PrimitiveTypes::mVector3, "lightDir", call("normalize", {lightDirection})).
        // variable(lightPos, GPUShaderDefinitions::PrimitiveTypes::mVector3, "lightPos", call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {"0"s,"0"s,"-100"s})).
        // variable(lightDir, GPUShaderDefinitions::PrimitiveTypes::mVector3, "lightDir", call("normalize", {lightPos.sub(fragPosition)})).
        variable(bias, GPUShaderDefinitions::PrimitiveTypes::mFloat, "bias", call("max", {Variable("0.0025").mul(paren(Variable("1.0").sub(call("dot", {normal, lightDir})))), {"0.0005"}})).
        // set(bias, call("clamp", {bias, "0.005"s, "0.01"s})).
        // set(bias, "0.0005"s).
        variable(currentDepth, GPUShaderDefinitions::PrimitiveTypes::mFloat, "currentDepth", projCoords.dot("z").sub(bias)).
        variable(shadow, GPUShaderDefinitions::PrimitiveTypes::mFloat, "shadow", currentDepth.great(closestDepth).ternary("0.5"s, "0.0"s)).
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
        variable(a, GPUShaderDefinitions::PrimitiveTypes::mFloat, "a", call("dFdx", {roughness.mul(roughness)})).
        variable(a2, GPUShaderDefinitions::PrimitiveTypes::mFloat, "a2", a.mul(a)).
        variable(NdotH, GPUShaderDefinitions::PrimitiveTypes::mFloat, "NdotH", call("max", {call("dot", {N, H}), {"0.0"}})).
        variable(NdotH2, GPUShaderDefinitions::PrimitiveTypes::mFloat, "NdotH2", NdotH.mul(NdotH)).
        variable(nom, GPUShaderDefinitions::PrimitiveTypes::mFloat, "nom", a2).
        variable(denom, GPUShaderDefinitions::PrimitiveTypes::mFloat, "denom", NdotH2.mul(paren(a2.sub("1"s)).add("1"s))).
        set(denom, Variable(GPUShaderDefinitions::Consts::mPI).mul(denom.mul(denom))).
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
        variable(r, GPUShaderDefinitions::PrimitiveTypes::mFloat, "r", roughness.add("1.0"s)).
        variable(k, GPUShaderDefinitions::PrimitiveTypes::mFloat, "k", r.mul(r).div("8.0"s)).
        variable(nom, GPUShaderDefinitions::PrimitiveTypes::mFloat, "nom", NdotV).
        variable(denom, GPUShaderDefinitions::PrimitiveTypes::mFloat, "denom", NdotV.mul(paren(Variable("1.0").sub(k)).add(k))).
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
        variable(NdotV, GPUShaderDefinitions::PrimitiveTypes::mFloat, "NdotV", call("max", {call("dot", {N, V}), {"0.0"}})).
        variable(NdotL, GPUShaderDefinitions::PrimitiveTypes::mFloat, "NdotL", call("max", {call("dot", {N, L}), {"0.0"}})).
        variable(ggx2, GPUShaderDefinitions::PrimitiveTypes::mFloat, "ggx2", call("geometrySchlickGGX", {NdotV, roughness})).
        variable(ggx1, GPUShaderDefinitions::PrimitiveTypes::mFloat, "ggx1", call("geometrySchlickGGX", {NdotL, roughness})).
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
        variable(L, GPUShaderDefinitions::PrimitiveTypes::mVector3, "L", call("normalize", {lightDirection})).
        variable(H, GPUShaderDefinitions::PrimitiveTypes::mVector3, "H", call("normalize", {V.add(L)})).
        variable(distance, GPUShaderDefinitions::PrimitiveTypes::mFloat, "distance", call("length", {lightDirection})).
        variable(attenuation, GPUShaderDefinitions::PrimitiveTypes::mFloat, "attenuation", Variable("1.0").div(paren(distance.mul(distance)))).
        variable(radiance, GPUShaderDefinitions::PrimitiveTypes::mVector3, "radiance", lightColor.mul(attenuation)).
        
        /*
            // Cook-Torrance BRDF
            float NDF = DistributionGGX(N, H, roughness);   
            float G   = GeometrySmith(N, V, L, roughness);      
            vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
            
            vec3 numerator    = NDF * G * F; 
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
            vec3 specular = numerator / denominator;
        */
        variable(NDF, GPUShaderDefinitions::PrimitiveTypes::mFloat, "NDF", call(mDistributionGGX, {N, H, roughness})).
        variable(G, GPUShaderDefinitions::PrimitiveTypes::mFloat, "G", call(mGeometrySmith, {N, V, L, roughness})).
        variable(F, GPUShaderDefinitions::PrimitiveTypes::mVector3, "F", call(mFresnelSchlick, {call("max",{call("dot", {V, H}), "0.0"s}), F0})).
        variable(numerator, GPUShaderDefinitions::PrimitiveTypes::mVector3, "numerator", NDF.mul(G).mul(F)).
        variable(denominator, GPUShaderDefinitions::PrimitiveTypes::mFloat, "denominator",
            Variable("4.0").
            mul(call("max", {call("dot", {N, V}), "0.0"s})).
            mul(call("max", {call("dot", {N, L}), "0.0"s})).
            add("0.0001"s)
        ).
        variable(specular, GPUShaderDefinitions::PrimitiveTypes::mVector3, "specular", numerator.div(denominator)).

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
        variable(kS, GPUShaderDefinitions::PrimitiveTypes::mVector3, "kS", F).
        variable(kD, GPUShaderDefinitions::PrimitiveTypes::mVector3, "kD", call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {"1.0"s}).sub(kS)).
        set(kD, kD.mul(paren(Variable("1.0").sub(metallic)))).

        /*
            // scale light by NdotL
            float NdotL = max(dot(N, L), 0.0);        

            // add to outgoing radiance Lo
            Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        */
        variable(NdotL, GPUShaderDefinitions::PrimitiveTypes::mFloat, "NdotL", call("max", {call("dot", {N, L}), "0.0"s})).
        variable(Lo, GPUShaderDefinitions::PrimitiveTypes::mVector3, "Lo", paren(paren(kD.mul(albedo).div(GPUShaderDefinitions::Consts::mPI).add(specular)).mul(radiance).mul(NdotL))).
        ret(Lo);

        shaderBuilder.get().function(funcCalculatePBRSingleLight);
    }

    {
        FunctionDefinition funcCalculatePBR(mCalculatePBR);
        Variable baseColor = funcCalculatePBR.mParameters[0];

        auto& globalDataBuffer = shaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData.mInstanceName);    
        Variable cameraPosition(globalDataBuffer.mGPUUniformBufferData.getScopedGPUVariableData(1));
        auto& inNormal = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mNormal);
        auto& fragPosition = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mFragPosition);
        auto& ligthsDataBuffer = shaderBuilder.get().getUniformBuffer(LightBuiltIn::mLightsBufferData.mInstanceName);    
        Variable pointLights(ligthsDataBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        Variable pointLightPos = {LightBuiltIn::mPointLightStructDefinition.mPrimitiveVariables[0]};
        Variable pointLightDiffuse = {LightBuiltIn::mPointLightStructDefinition.mPrimitiveVariables[1]};

        Variable spotLights(ligthsDataBuffer.mGPUUniformBufferData.getScopedGPUVariableData(1));
        Variable spotLightPos = {LightBuiltIn::mSpotLightStructDefinition.mPrimitiveVariables[0]};
        Variable spotLightDiffuse = {LightBuiltIn::mSpotLightStructDefinition.mPrimitiveVariables[1]};
        Variable spotLightInnerCutOff = {LightBuiltIn::mSpotLightStructDefinition.mPrimitiveVariables[2]};
        Variable spotLightOuterCutOff = {LightBuiltIn::mSpotLightStructDefinition.mPrimitiveVariables[3]};

        Variable directionalLight(ligthsDataBuffer.mGPUUniformBufferData.getScopedGPUVariableData(2));
        Variable directionalLightDirection = {LightBuiltIn::mDirectionalLightStructDefinition.mPrimitiveVariables[0]};
        Variable directionalLightDiffuse = {LightBuiltIn::mDirectionalLightStructDefinition.mPrimitiveVariables[1]};

        Variable propertiesBlock(mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
        Variable shaderBaseColor = {mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
        Variable shaderMetallic = {mPropertiesBlockStructDefinition.mPrimitiveVariables[1]};
        Variable shaderRoughness = {mPropertiesBlockStructDefinition.mPrimitiveVariables[2]};
        auto& shaderInstanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mShaderInstanceID);

        Variable roughness;
        Variable metallic;
        funcCalculatePBR.body().
        variable(roughness, GPUShaderDefinitions::PrimitiveTypes::mFloat, "roughness", propertiesBlock.at(shaderInstanceId).dot(shaderRoughness)).
        variable(metallic, GPUShaderDefinitions::PrimitiveTypes::mFloat, "metallic", propertiesBlock.at(shaderInstanceId).dot(shaderMetallic));

        auto& textureHandlerMetallicRoughness = shaderBuilder.get().getAttribute(GPUShaderDefinitions::Uniforms::getTextureHandler(TextureBindingNamesPBR::smMetallicRoughness).mName);
        auto& texturesBuffer = shaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mTextures.mInstanceName);    
        Variable textures(texturesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mTextureCoords.at(0));

        if(inTextureCoord.isValid())
        {
            Variable metallicRoughnessPack;
            funcCalculatePBR.body().
            // ifBlock(textureHandlerMetallicRoughness.notEq("0"s)).
                variable(metallicRoughnessPack, GPUShaderDefinitions::PrimitiveTypes::mVector4, "metallicRoughnessPack", call("texture", {/*textures.at(textureHandlerMetallicRoughness)*/textureHandlerMetallicRoughness, inTextureCoord})).
                set(roughness, metallicRoughnessPack.dot("g")).
                set(metallic, metallicRoughnessPack.dot("b"));
            // end();
        }

        // base color gamma correct
        Variable albedo;
        funcCalculatePBR.body().
        variable(albedo, GPUShaderDefinitions::PrimitiveTypes::mVector3, "albedo", call("pow", {baseColor, "vec3(2.2)"s}));

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
        variable(N, GPUShaderDefinitions::PrimitiveTypes::mVector3, "N", call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {{"0.0"}, {"0.0"}, {"0.0"}}));

        auto& textureHandler = shaderBuilder.get().getAttribute(GPUShaderDefinitions::Uniforms::getTextureHandler(TextureBindingNamesPBR::smNormal).mName);
        funcCalculatePBR.body().
        // ifBlock(textureHandler.notEq("0"s)).
        //     set(N, call(mGetNormalFromMap, {})).
        // end().
        // elseBlock().
            set(N, inNormal);
        // end();

        funcCalculatePBR.body().
        set(N, call("normalize", {N}));

        Variable V;
        Variable F0;
        funcCalculatePBR.body().
        variable(V, GPUShaderDefinitions::PrimitiveTypes::mVector3, "V", call("normalize", {cameraPosition.sub(fragPosition)})).
        variable(F0, GPUShaderDefinitions::PrimitiveTypes::mVector3, "F0", call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {"0.04"s})).
        set(F0, call("mix", {F0, albedo, metallic}));

        /*
            // reflectance equation
            vec3 Lo = vec3(0.0);
        */
        Variable Lo;
        funcCalculatePBR.body().
        variable(Lo, GPUShaderDefinitions::PrimitiveTypes::mVector3, "Lo", call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {"0"s}));


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
        variable(lightDirection, GPUShaderDefinitions::PrimitiveTypes::mVector3, "lightDirection", directionalLight.dot(directionalLightDirection)).
        variable(lightDiffuse, GPUShaderDefinitions::PrimitiveTypes::mVector3, "lightDiffuse", directionalLight.dot(directionalLightDiffuse));
        
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
        variable(ambient, GPUShaderDefinitions::PrimitiveTypes::mVector3, "ambient", call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {"0.03"s}).mul(albedo)/*.mul(ao)*/);

        /*
            vec3 color = ambient + Lo;

            // HDR tonemapping
            color = color / (color + vec3(1.0));
            // gamma correct
            color = pow(color, vec3(1.0/2.2)); 
        */

        Variable PBRFinalColor;
        funcCalculatePBR.body().
        variable(PBRFinalColor, GPUShaderDefinitions::PrimitiveTypes::mVector3, "PBRFinalColor", ambient.add(Lo)).
        set(PBRFinalColor, PBRFinalColor.div(paren(PBRFinalColor.add(call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {"1.0"s}))))).
        set(PBRFinalColor, call("pow", {PBRFinalColor, call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {"1.0/2.2"s})}));

        Variable sampler = GPUShaderDefinitions::Uniforms::getSampler(TextureBindingNamesPBR::smShadowMap);
        if(mFramebufferBindings.contains(TextureBindingNamesPBR::smShadowMap))
        {
            auto& fragPositionLight = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mFragPositionLight);
            
            Variable shadow;
            funcCalculatePBR.body().
            variable(shadow, GPUShaderDefinitions::PrimitiveTypes::mFloat, "shadow", call(mCalculateShadow, {fragPositionLight, lightDirection})).
            set(PBRFinalColor, PBRFinalColor.mul(paren(Variable("1.0").sub(shadow))));
        }

        funcCalculatePBR.body().
        ret(call(GPUShaderDefinitions::PrimitiveTypes::mVector4, {PBRFinalColor, {"1"}}));

        shaderBuilder.get().function(funcCalculatePBR);
    }
}