#include "Graphics/Shaders/ShaderBuilder.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Buffers/GPUBuffersLayout.hpp"
#include "Graphics/Buffers/GPUDefinitions.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Shaders/ShaderBuilderFunctionsLibrary.hpp"

ShaderBuilder::ShaderBuilder()
{

}

void ShaderBuilder::createShader(const GPUBuffersLayout& gpuBuffersLayout, Ptr<const Material> material)
{
    using namespace ShaderBuilderNodes;
    using namespace ShaderBuilderNodes::Expressions;
    
    const std::vector<GPUVariableData>& consts = material.get().getConsts();
    FOR_LIST(it, consts)
    {
        const GPUVariableData& constVar = *it;
        get().attribute(GPUStorage::CONST, constVar);
    }

    const std::vector<GPUBuffer>& gpuBuffers = gpuBuffersLayout.getBuffers();
    FOR_LIST(it, gpuBuffers)
    {
        const GPUBuffer& gpuBuffer = *it;
        get().attribute(GPUStorage::IN, gpuBuffer.getAttributeLocation(), gpuBuffer.mData.mGPUVariableData);
    }

    const std::vector<GPUVariableData>& uniforms = material.get().getUniforms();
    FOR_LIST(it, uniforms)
    {
        const GPUVariableData& uniformVar = *it;
        get().attribute(GPUStorage::UNIFORM, uniformVar);
    }

    const std::vector<GPUVariableData>& outputs = material.get().getVertexOutputs();
    FOR_LIST(it, outputs)
    {
        const GPUVariableData& outputVar = *it;
        get().attribute(GPUStorage::OUT, outputVar);
    }

    ShaderBuilderFunctionsLibrary shaderBuilderFunctionsLibrary;

    FunctionDefinition func = shaderBuilderFunctionsLibrary.getFunctionCalculateSkinnedPosition(get());
    get().function(func);

    // retrieve the needed attributes
    auto& position = get().getAttribute(GPUBuiltIn::VertexInput::mPosition.mName);
    auto& modelMatrix = get().getAttribute(GPUBuiltIn::VertexInput::mModelMatrix.mName);
    auto& color = get().getAttribute(GPUBuiltIn::VertexInput::mColor.mName);
    auto& texture = get().getAttribute(GPUBuiltIn::VertexInput::mTexture.mName);    

    auto& projectionMatrix = get().getAttribute(GPUBuiltIn::Uniforms::mProjectionMatrix.mName);
    auto& viewMatrix = get().getAttribute(GPUBuiltIn::Uniforms::mViewMatrix.mName);
    auto& isInstanced = get().getAttribute(GPUBuiltIn::Uniforms::mIsInstanced.mName);
    
    auto& outColor = get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    auto& outTexture = get().getAttribute(GPUBuiltIn::VertexOutput::mTexture.mName);

    auto& mainFunc = get().function("void", "main");

    Variable finalPositon;
    Variable PVMatrix;

    mainFunc.body().
    variable(finalPositon, "vec4", "finalPositon", call(func.mName, {position, {"1.0f"}})).
    variable(PVMatrix, "mat4", "PV_Matrix", projectionMatrix.mul(viewMatrix)).
    ifBlock(isInstanced).
        set(finalPositon, modelMatrix.mul(finalPositon)).
    end().
    set(GPUBuiltIn::VertexOutput::mPosition, PVMatrix.mul(finalPositon)).
    set(outColor, color).
    set(outTexture, texture);
}

std::string ShaderBuilder::getCode() const
{
    std::string code = "";
    auto codeLines = mProgram.toLines(0);
    FOR_LIST(it, codeLines)
    {
        code.append((*it) + "\n");
    }

    return code;
}