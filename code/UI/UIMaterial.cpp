#include "UI/UIMaterial.hpp"
#include "Graphics/Material/Material.hpp"

using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

std::vector<GPUStructDefinition::GPUStructVariable> MaterialRuntimeUI::generateMaterialPropertiesBlock()
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

void MaterialRuntimeUI::fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const
{
    MaterialRuntimeDefault::fragmentShaderBaseColor(shaderBuilder);

    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain);
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor);
    Variable instanceColor = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    
    Variable baseColor = shaderBuilder.getVariableFromCache("baseColor");

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    mainFunc.body().
    set(baseColor, propertiesBlock.at(materialInstanceId).dot(instanceColor));

    mainFunc.body().
    set(outColor, baseColor);
}

void MaterialRuntimeUI::vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const
{
    MaterialRuntimeDefault::vertexShaderCalculateTextureCoordinateOutput(shaderBuilder);

    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain);

    auto& outTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord);

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable textureRegionLeftTop = {getPropertiesBlockStructDefinition().mPrimitiveVariables[1]};
    Variable textureRegionSize = {getPropertiesBlockStructDefinition().mPrimitiveVariables[2]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mMaterialInstanceID);
    mainFunc.body().
    set(outTextureCoord, call(GPUBuiltIn::PrimitiveTypes::mVector2,
    {
        outTextureCoord.dot("x").mul(propertiesBlock.at(materialInstanceId).dot(textureRegionSize).dot("x")).add(propertiesBlock.at(materialInstanceId).dot(textureRegionLeftTop).dot("x")),
        outTextureCoord.dot("y").mul(propertiesBlock.at(materialInstanceId).dot(textureRegionSize).dot("y")).add(propertiesBlock.at(materialInstanceId).dot(textureRegionLeftTop).dot("y"))
    }));
}

void MaterialRuntimeUI::vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain);
    
    Variable finalPositon = shaderBuilder.getVariableFromCache("finalPositon");

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable depth = {getPropertiesBlockStructDefinition().mPrimitiveVariables[3]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mMaterialInstanceID);
    if(mMaterial->getMaterialData().mUseDepth)
    {
        mainFunc.body().
        set(finalPositon.dot("z"), propertiesBlock.at(materialInstanceId).dot(depth));
    }
}