#include "UI/UIMaterial.hpp"
#include "Graphics/Material/Material.hpp"

using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

std::vector<GPUStructDefinition::GPUStructVariable> ShaderUI::generateMaterialPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {GPUBuiltIn::PrimitiveTypes::mVector4, "color"},
        {GPUBuiltIn::PrimitiveTypes::mVector2, "textureRegionLeftTop"},
        {GPUBuiltIn::PrimitiveTypes::mVector2, "textureRegionSize"},
        {GPUBuiltIn::PrimitiveTypes::mInt, "depth"},
    };

    return propertiesBlock;
}

void ShaderUI::fragmentShaderCode(ShaderBuilder& shaderBuilder) const
{
    // ShaderDefault::fragmentShaderCode(shaderBuilder);

    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor);
    Variable instanceColor = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
    Variable propertiesBlock(getShaderData().mPropertiesBlockSharedBufferData.getScopedGPUVariableData(0));
    
    Variable baseColor;
    shaderBuilder.getMain().
    variable(baseColor, GPUBuiltIn::PrimitiveTypes::mVector4, "baseColor", propertiesBlock.at(materialInstanceId).dot(instanceColor));

    shaderBuilder.getMain().
    set(outColor, baseColor);
    
    if(getShaderData().mMaterial->hasTexture(TextureMap::BASE_COLOR))
    {
        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord);
        auto& sampler = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getSampler(EnumsManager::toString<TextureMap>(TextureMap::BASE_COLOR)));
        shaderBuilder.getMain().
        set(outColor, call("texture", {sampler, inTextureCoord}));

        shaderBuilder.getMain().
        ifBlock(outColor.dot("r").add(outColor.dot("g").add(outColor.dot("b"))).eq({"0"})).
            line("discard").
        end();

        if(getShaderData().mMaterial->getMaterialData().mIsFont)
        {
            shaderBuilder.getMain().
            set(outColor.dot("a"), outColor.dot("r"));
            
            shaderBuilder.getMain().
            set(outColor.dot("r"), baseColor.dot("r")).
            set(outColor.dot("g"), baseColor.dot("g")).
            set(outColor.dot("b"), baseColor.dot("b"));
        }
    }
}

void ShaderUI::vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const
{
    ShaderDefault::vertexShaderCalculateTextureCoordinateOutput(shaderBuilder);

    auto& outTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord);

    Variable propertiesBlock(getShaderData().mPropertiesBlockSharedBufferData.getScopedGPUVariableData(0));
    Variable textureRegionLeftTop = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[1]};
    Variable textureRegionSize = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[2]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mMaterialInstanceID);
    shaderBuilder.getMain().
    set(outTextureCoord, call(GPUBuiltIn::PrimitiveTypes::mVector2,
    {
        outTextureCoord.dot("x").mul(propertiesBlock.at(materialInstanceId).dot(textureRegionSize).dot("x")).add(propertiesBlock.at(materialInstanceId).dot(textureRegionLeftTop).dot("x")),
        outTextureCoord.dot("y").mul(propertiesBlock.at(materialInstanceId).dot(textureRegionSize).dot("y")).add(propertiesBlock.at(materialInstanceId).dot(textureRegionLeftTop).dot("y"))
    }));
}

void ShaderUI::vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const
{    
    Variable finalPositon = shaderBuilder.getVariableFromCache("finalPositon");

    Variable propertiesBlock(getShaderData().mPropertiesBlockSharedBufferData.getScopedGPUVariableData(0));
    Variable depth = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[3]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mMaterialInstanceID);
    shaderBuilder.getMain().
    set(finalPositon.dot("z"), propertiesBlock.at(materialInstanceId).dot(depth));
}