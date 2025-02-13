#include "UI/UIShader.hpp"

using namespace GPUShaderBuilderNodes;
using namespace GPUShaderBuilderNodes::Expressions;

void GPUShaderUI::setSharedGPUShaderPropertiesBlock()
{
    mSharedGPUShaderPropertiesBlockBuffer.set<GPUShaderPropertiesBlockUI>();
    mSharedGPUShaderPropertiesBlockClass = ClassManager::getClassMetadata<GPUShaderPropertiesBlockUI>().mClassDefinition;
}

std::vector<GPUStructDefinition::GPUStructVariable> GPUShaderUI::generateGPUShaderPropertiesBlock()
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

void GPUShaderUI::fragmentGPUShaderCode(GPUShaderBuilder& GPUShaderBuilder) const
{
    // GPUShaderDefault::fragmentGPUShaderCode(GPUShaderBuilder);

    auto& shaderPropertiesInstanceId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mGPUShaderPropertiesInstanceID);
    auto& outColor = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentOutput::mColor);
    Variable instanceColor = {mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
    Variable propertiesBlock(mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
    
    Variable baseColor;
    GPUShaderBuilder.getMain().
    variable(baseColor, GPUShaderDefinitions::PrimitiveTypes::mVector4, "baseColor", propertiesBlock.at(shaderPropertiesInstanceId).dot(instanceColor));

    GPUShaderBuilder.getMain().
    set(outColor, baseColor);
    
    // NEXT: Restore ui/font texture
    auto& inTextureCoord = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mTextureCoords.at(0));
    auto& textureHandler = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::Uniforms::getTextureHandler(TextureBindingNames::smBaseColor));
    if(textureHandler.isValid())
    {
        if(inTextureCoord.isValid())
        {
            // auto& texturesBuffer = GPUShaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mTextures.mInstanceName);    
            // Variable textures(texturesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
            GPUShaderBuilder.getMain().
            // ifBlock(textureHandler.notEq("0"s)).
                set(outColor, call("texture", {/*textures.at(textureHandler)*/textureHandler, inTextureCoord})).
                ifBlock(outColor.dot("r").add(outColor.dot("g").add(outColor.dot("b"))).eq({"0"})).
                    line("discard").
                end();
            // end();
        }
    }
}

void GPUShaderUI::vertexGPUShaderCalculateTextureCoordinateOutput(GPUShaderBuilder& GPUShaderBuilder) const
{
    GPUShaderDefault::vertexGPUShaderCalculateTextureCoordinateOutput(GPUShaderBuilder);
    auto& textureCoord = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0));
    if(textureCoord.isValid())
    {
        auto& outTextureCoord = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mTextureCoords.at(0));

        Variable propertiesBlock(mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
        Variable textureRegionLeftTop = {mPropertiesBlockStructDefinition.mPrimitiveVariables[1]};
        Variable textureRegionSize = {mPropertiesBlockStructDefinition.mPrimitiveVariables[2]};
        auto& shaderPropertiesInstanceId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mGPUShaderPropertiesInstanceID);
        GPUShaderBuilder.getMain().
        set(outTextureCoord, call(GPUShaderDefinitions::PrimitiveTypes::mVector2,
        {
            outTextureCoord.dot("x").mul(propertiesBlock.at(shaderPropertiesInstanceId).dot(textureRegionSize).dot("x")).add(propertiesBlock.at(shaderPropertiesInstanceId).dot(textureRegionLeftTop).dot("x")),
            outTextureCoord.dot("y").mul(propertiesBlock.at(shaderPropertiesInstanceId).dot(textureRegionSize).dot("y")).add(propertiesBlock.at(shaderPropertiesInstanceId).dot(textureRegionLeftTop).dot("y"))
        }));
    }
}

void GPUShaderUI::vertexGPUShaderCalculatePositionOutputCustom(GPUShaderBuilder& GPUShaderBuilder) const
{    
    Variable finalPositon = GPUShaderBuilder.getVariableFromCache("finalPositon");

    Variable propertiesBlock(mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
    Variable depth = {mPropertiesBlockStructDefinition.mPrimitiveVariables[3]};
    auto& shaderPropertiesInstanceId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mGPUShaderPropertiesInstanceID);
    GPUShaderBuilder.getMain().
    set(finalPositon.dot("z"), propertiesBlock.at(shaderPropertiesInstanceId).dot(depth));
}

void GPUShaderUIFont::fragmentGPUShaderCode(GPUShaderBuilder& GPUShaderBuilder) const
{
    GPUShaderUI::fragmentGPUShaderCode(GPUShaderBuilder);

    auto& shaderPropertiesInstanceId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mGPUShaderPropertiesInstanceID);
    auto& outColor = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentOutput::mColor);
    Variable instanceColor = {mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};
    Variable propertiesBlock(mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
    
    Variable baseColor;
    GPUShaderBuilder.getMain().
    variable(baseColor, GPUShaderDefinitions::PrimitiveTypes::mVector4, "baseColorFont", propertiesBlock.at(shaderPropertiesInstanceId).dot(instanceColor));
    
    GPUShaderBuilder.getMain().
    set(outColor.dot("a"), outColor.dot("r"));
    
    GPUShaderBuilder.getMain().
    set(outColor.dot("r"), baseColor.dot("r")).
    set(outColor.dot("g"), baseColor.dot("g")).
    set(outColor.dot("b"), baseColor.dot("b"));
}