#include "UI/UIMaterial.hpp"
#include "Graphics/Material/Material.hpp"

using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

std::vector<GPUStructDefinition::GPUStructVariable> ShaderUI::generateMaterialPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {GPUShaderDefinitions::PrimitiveTypes::mVector4, "color"},
        {GPUShaderDefinitions::PrimitiveTypes::mVector2, "textureRegionLeftTop"},
        {GPUShaderDefinitions::PrimitiveTypes::mVector2, "textureRegionSize"},
        {GPUShaderDefinitions::PrimitiveTypes::mInt, "depth"},
    };

    return propertiesBlock;
}

void ShaderUI::fragmentShaderCode(ShaderBuilder& shaderBuilder) const
{
    // ShaderDefault::fragmentShaderCode(shaderBuilder);

    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mMaterialInstanceID);
    auto& outColor = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentOutput::mColor);
    Variable instanceColor = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
    Variable propertiesBlock(getShaderData().mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
    
    Variable baseColor;
    shaderBuilder.getMain().
    variable(baseColor, GPUShaderDefinitions::PrimitiveTypes::mVector4, "baseColor", propertiesBlock.at(materialInstanceId).dot(instanceColor));

    shaderBuilder.getMain().
    set(outColor, baseColor);
    
    // NEXT: Restore ui/font texture
    // auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mTextureCoords.at(0));
    // auto& textureHandler = shaderBuilder.get().getAttribute(GPUShaderDefinitions::Uniforms::getTextureHandler(TextureBindingNames::smBaseColor));
    // auto& texturesBuffer = shaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mTextures.mInstanceName);    
    // Variable textures(texturesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));

    // shaderBuilder.getMain().
    // // ifBlock(textureHandler.notEq("0"s)).
    //     set(outColor, call("texture", {/*textures.at(textureHandler)*/textureHandler, inTextureCoord})).
    //     ifBlock(outColor.dot("r").add(outColor.dot("g").add(outColor.dot("b"))).eq({"0"})).
    //         line("discard").
    //     end();
    // // end();
}

void ShaderUI::vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const
{
    ShaderDefault::vertexShaderCalculateTextureCoordinateOutput(shaderBuilder);
    auto& textureCoord = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0));
    if(textureCoord.isValid())
    {
        auto& outTextureCoord = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mTextureCoords.at(0));

        Variable propertiesBlock(getShaderData().mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
        Variable textureRegionLeftTop = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[1]};
        Variable textureRegionSize = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[2]};
        auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mMaterialInstanceID);
        shaderBuilder.getMain().
        set(outTextureCoord, call(GPUShaderDefinitions::PrimitiveTypes::mVector2,
        {
            outTextureCoord.dot("x").mul(propertiesBlock.at(materialInstanceId).dot(textureRegionSize).dot("x")).add(propertiesBlock.at(materialInstanceId).dot(textureRegionLeftTop).dot("x")),
            outTextureCoord.dot("y").mul(propertiesBlock.at(materialInstanceId).dot(textureRegionSize).dot("y")).add(propertiesBlock.at(materialInstanceId).dot(textureRegionLeftTop).dot("y"))
        }));
    }
}

void ShaderUI::vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const
{    
    Variable finalPositon = shaderBuilder.getVariableFromCache("finalPositon");

    Variable propertiesBlock(getShaderData().mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
    Variable depth = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[3]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mMaterialInstanceID);
    shaderBuilder.getMain().
    set(finalPositon.dot("z"), propertiesBlock.at(materialInstanceId).dot(depth));
}

void ShaderUIFont::fragmentShaderCode(ShaderBuilder& shaderBuilder) const
{
    ShaderUI::fragmentShaderCode(shaderBuilder);

    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mMaterialInstanceID);
    auto& outColor = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentOutput::mColor);
    Variable instanceColor = {getShaderData().mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
    Variable propertiesBlock(getShaderData().mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
    
    Variable baseColor;
    shaderBuilder.getMain().
    variable(baseColor, GPUShaderDefinitions::PrimitiveTypes::mVector4, "baseColorFont", propertiesBlock.at(materialInstanceId).dot(instanceColor));
    
    shaderBuilder.getMain().
    set(outColor.dot("a"), outColor.dot("r"));
    
    shaderBuilder.getMain().
    set(outColor.dot("r"), baseColor.dot("r")).
    set(outColor.dot("g"), baseColor.dot("g")).
    set(outColor.dot("b"), baseColor.dot("b"));
}