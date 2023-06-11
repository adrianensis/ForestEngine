#include "Graphics/Shaders/ShaderBuilder.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Buffers/GPUBuffersLayout.hpp"
#include "Graphics/Buffers/GPUDefinitions.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Shaders/ShaderBuilderFunctionsLibrary.hpp"

ShaderBuilder::ShaderBuilder()
{

}

void ShaderBuilder::createVertexShader(const GPUBuffersLayout& gpuBuffersLayout, Ptr<const Material> material)
{
    using namespace ShaderBuilderNodes;
    using namespace ShaderBuilderNodes::Expressions;
    
    const std::vector<GPUVariableData>& consts = material->getConsts();
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

    const std::vector<GPUVariableData>& uniforms = material->getUniforms();
    FOR_LIST(it, uniforms)
    {
        const GPUVariableData& uniformVar = *it;
        get().attribute(GPUStorage::UNIFORM, uniformVar);
    }

    const std::vector<GPUVariableData>& outputs = material->getVertexOutputs();
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
    variable(finalPositon, "vec4", "finalPositon", call(func.mName, {call("vec4", {position, {"1.0f"}})})).
    variable(PVMatrix, "mat4", "PV_Matrix", projectionMatrix.mul(viewMatrix)).
    set(finalPositon, modelMatrix.mul(finalPositon)).
    set(GPUBuiltIn::VertexOutput::mPosition, PVMatrix.mul(finalPositon)).
    set(outColor, color).
    set(outTexture, texture);
}

void ShaderBuilder::createFragmentShader(const GPUBuffersLayout& gpuBuffersLayout, Ptr<const Material> material)
{
    using namespace ShaderBuilderNodes;
    using namespace ShaderBuilderNodes::Expressions;
    
    const std::vector<GPUVariableData>& consts = material->getConsts();
    FOR_LIST(it, consts)
    {
        const GPUVariableData& constVar = *it;
        get().attribute(GPUStorage::CONST, constVar);
    }

    const std::vector<GPUVariableData>& uniforms = material->getUniforms();
    FOR_LIST(it, uniforms)
    {
        const GPUVariableData& uniformVar = *it;
        get().attribute(GPUStorage::UNIFORM, uniformVar);
    }

    const std::vector<GPUVariableData>& vertexOutputs = material->getVertexOutputs();
    FOR_LIST(it, vertexOutputs)
    {
        const GPUVariableData& outputVar = *it;
        get().attribute(GPUStorage::IN, outputVar);
    }

    const std::vector<GPUVariableData>& fragmentOutputs = material->getFragmentOutputs();
    FOR_LIST(it, fragmentOutputs)
    {
        const GPUVariableData& outputVar = *it;
        get().attribute(GPUStorage::OUT, outputVar);
    }

    auto& inColor = get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    auto& inTexture = get().getAttribute(GPUBuiltIn::VertexOutput::mTexture.mName);
    auto& outColor = get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);

    auto& sampler = get().getAttribute(GPUBuiltIn::Uniforms::mSampler.mName);
    auto& hasTexture = get().getAttribute(GPUBuiltIn::Uniforms::mHasTexture.mName);
    auto& alphaEnabled = get().getAttribute(GPUBuiltIn::Uniforms::mAlphaEnabled.mName);
    auto& useVertexColor = get().getAttribute(GPUBuiltIn::Uniforms::mUseVertexColor.mName);
    auto& baseColor = get().getAttribute(GPUBuiltIn::Uniforms::mBaseColor.mName);

    auto& mainFunc = get().function("void", "main");

    Variable t;

    mainFunc.body().
    variable(t, "vec2", "t", inTexture).
    ifBlock(hasTexture).
        set(GPUBuiltIn::FragmentOutput::mColor, call("texture2D", {sampler, t})).
    end().
    elseBlock().
        ifBlock(useVertexColor).
            set(outColor.dot("r"), inColor.dot("r")).
            set(outColor.dot("g"), inColor.dot("g")).
            set(outColor.dot("b"), inColor.dot("b")).
            set(outColor.dot("a"), inColor.dot("a")).
        end().
        elseBlock().
            set(outColor.dot("r"), baseColor.dot("r")).
            set(outColor.dot("g"), baseColor.dot("g")).
            set(outColor.dot("b"), baseColor.dot("b")).
            set(outColor.dot("a"), baseColor.dot("a")).
        end().
    end().
    ifBlock(alphaEnabled, "&&", outColor.dot("r").add(outColor.dot("g").add(outColor.dot("b"))).eq({"0"})).
        line("discard").
    end();
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