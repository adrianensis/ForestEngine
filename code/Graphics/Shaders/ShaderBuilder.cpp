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

    const std::vector<GPUBuffer>& gpuBuffers = gpuBuffersLayout.getBuffers();
    FOR_LIST(it, gpuBuffers)
    {
        const GPUBuffer& gpuBuffer = *it;
        get().attribute(GPUStorage::IN, gpuBuffer.getAttributeLocation(), {gpuBuffer.mData.mGPUVariableData.mGPUDataType.mTypeName, gpuBuffer.mData.mGPUVariableData.mAttributeName});
    }

    const std::vector<GPUVariableData>& uniforms = material.get().getUniforms();
    FOR_LIST(it, uniforms)
    {
        const GPUVariableData& uniformVar = *it;
        get().attribute(GPUStorage::UNIFORM, {uniformVar.mGPUDataType.mTypeName, uniformVar.mAttributeName, uniformVar.mValue, uniformVar.mArraySize});
    }

    const std::vector<GPUVariableData>& consts = material.get().getConsts();
    FOR_LIST(it, consts)
    {
        const GPUVariableData& constVar = *it;
        get().attribute(GPUStorage::CONST, {constVar.mGPUDataType.mTypeName, constVar.mAttributeName, constVar.mValue, constVar.mArraySize});
    }

    const std::vector<GPUVariableData>& outputs = material.get().getVertexOutputs();
    FOR_LIST(it, outputs)
    {
        const GPUVariableData& outputVar = *it;
        get().attribute(GPUStorage::OUT, {outputVar.mGPUDataType.mTypeName, outputVar.mAttributeName, outputVar.mValue, outputVar.mArraySize});
    }

    ShaderBuilderFunctionsLibrary shaderBuilderFunctionsLibrary;

    FunctionDefinition func = shaderBuilderFunctionsLibrary.getFunctionCalculateSkinnedPosition(get());
    get().function(func);

    // // retrieve the needed attributes
    auto& position = get().getAttribute(GPUBuiltIn::VertexInput::mPosition.mAttributeName);
    auto& mainFunc = get().function("void", "main");

    Variable finalPositon;

    mainFunc.body().
    variable(finalPositon, "vec4", "finalPositon", call(func.mName, {position, {"1.0f"}}));
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