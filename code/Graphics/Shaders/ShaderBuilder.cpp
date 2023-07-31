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
    
    const std::vector<GPUAttributeData>& consts = material->getConsts();
    FOR_LIST(it, consts)
    {
        const GPUAttributeData& constVar = *it;
        get().attribute(constVar);
    }

    const std::vector<GPUBuffer>& gpuBuffers = gpuBuffersLayout.getBuffers();
    FOR_LIST(it, gpuBuffers)
    {
        const GPUBuffer& gpuBuffer = *it;
        get().attribute({gpuBuffer.mData.mGPUAttributeData, gpuBuffer.getAttributeLocation()});
    }

    const std::vector<GPUAttributeData>& uniforms = material->getUniforms();
    FOR_LIST(it, uniforms)
    {
        const GPUAttributeData& uniformVar = *it;
        get().attribute(uniformVar);
    }

    const std::vector<GPUAttributeData>& outputs = material->getVertexOutputs();
    FOR_LIST(it, outputs)
    {
        const GPUAttributeData& outputVar = *it;
        get().attribute(outputVar);
    }

    ShaderBuilderFunctionsLibrary shaderBuilderFunctionsLibrary;

    FunctionDefinition functionCalculateSkinnedPosition = shaderBuilderFunctionsLibrary.getFunctionCalculateSkinnedPosition(get(), material);
    if(material->getMaterialData().mIsSkinned)
    {
        get().function(functionCalculateSkinnedPosition);
    }

    // retrieve the needed attributes
    auto& position = get().getAttribute(GPUBuiltIn::VertexInput::mPosition.mName);
    auto& modelMatrix = get().getAttribute(GPUBuiltIn::VertexInput::mModelMatrix.mName);
    auto& color = get().getAttribute(GPUBuiltIn::VertexInput::mColor.mName);
    auto& textureCoord = get().getAttribute(GPUBuiltIn::VertexInput::mTextureCoord.mName);    

    auto& projectionMatrix = get().getAttribute(GPUBuiltIn::Uniforms::mProjectionMatrix.mName);
    auto& viewMatrix = get().getAttribute(GPUBuiltIn::Uniforms::mViewMatrix.mName);
    auto& isInstanced = get().getAttribute(GPUBuiltIn::Uniforms::mIsInstanced.mName);
    
    auto& outColor = get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    auto& outTextureCoord = get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord.mName);

    auto& mainFunc = get().function("void", "main");

    Variable finalPositon;
    Variable PVMatrix;

    mainFunc.body().
    variable(finalPositon, "vec4", "finalPositon", call("vec4", {position, {"1.0f"}}));
    
    if(material->getMaterialData().mIsSkinned)
    {
        mainFunc.body().
        set(finalPositon, call(functionCalculateSkinnedPosition.mName, {finalPositon}));
    }

    mainFunc.body().variable(PVMatrix, "mat4", "PV_Matrix", projectionMatrix.mul(viewMatrix)).
    set(finalPositon, modelMatrix.mul(finalPositon)).
    set(GPUBuiltIn::VertexOutput::mPosition, PVMatrix.mul(finalPositon)).
    set(outTextureCoord, textureCoord);

    if(material->getMaterialData().mUseVertexColor)
    {
       mainFunc.body().
       set(outColor, color);
    }
}

void ShaderBuilder::createFragmentShader(const GPUBuffersLayout& gpuBuffersLayout, Ptr<const Material> material)
{
    using namespace ShaderBuilderNodes;
    using namespace ShaderBuilderNodes::Expressions;
    
    const std::vector<GPUAttributeData>& consts = material->getConsts();
    FOR_LIST(it, consts)
    {
        const GPUAttributeData& constVar = *it;
        get().attribute(constVar);
    }

    const std::vector<GPUAttributeData>& uniforms = material->getUniforms();
    FOR_LIST(it, uniforms)
    {
        const GPUAttributeData& uniformVar = *it;
        get().attribute(uniformVar);
    }

    const std::vector<GPUAttributeData>& fragmentInputs = material->getFragmentInputs();
    FOR_LIST(it, fragmentInputs)
    {
        const GPUAttributeData& inputVar = *it;
        get().attribute(inputVar);
    }

    const std::vector<GPUAttributeData>& fragmentOutputs = material->getFragmentOutputs();
    FOR_LIST(it, fragmentOutputs)
    {
        const GPUAttributeData& outputVar = *it;
        get().attribute(outputVar);
    }

    auto& inColor = get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    auto& inTextureCoord = get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord.mName);
    auto& outColor = get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);

    auto& sampler = get().getAttribute(GPUBuiltIn::Uniforms::mSampler.mName);
    auto& hasTexture = get().getAttribute(GPUBuiltIn::Uniforms::mHasTexture.mName);
    auto& alphaEnabled = get().getAttribute(GPUBuiltIn::Uniforms::mAlphaEnabled.mName);
    auto& useColorAsTint = get().getAttribute(GPUBuiltIn::Uniforms::mUseColorAsTint.mName);
    auto& baseColor = get().getAttribute(GPUBuiltIn::Uniforms::mBaseColor.mName);

    auto& mainFunc = get().function("void", "main");

    Variable t;
    Variable color;

    mainFunc.body().
    variable(color, "vec4", "color");

    if(material->getMaterialData().mUseVertexColor)
    {
        mainFunc.body().
        set(color, inColor);
    }
    else
    {
        mainFunc.body().
        set(color, baseColor);
    }

    mainFunc.body().
    set(outColor, color).
    ifBlock(hasTexture).
        set(outColor, call("texture2D", {sampler, inTextureCoord})).
        ifBlock(useColorAsTint).
            set(outColor.dot("r"), outColor.dot("r").add(color.dot("r"))).
            set(outColor.dot("g"), outColor.dot("g").add(color.dot("g"))).
            set(outColor.dot("b"), outColor.dot("b").add(color.dot("b"))).
            set(outColor.dot("a"), outColor.dot("a").add(color.dot("a"))).
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