#include "Graphics/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/GPU/GPUVertexBuffersLayout.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/ShaderBuilder/ShaderBuilderFunctionsLibrary.hpp"

ShaderBuilder::ShaderBuilder()
{

}

void ShaderBuilder::createVertexShader(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material)
{
    using namespace ShaderBuilderNodes;
    using namespace ShaderBuilderNodes::Expressions;

    const std::vector<GPUStructDefinition>& structDefinitions = material->getMaterialShaderVariables().mStructDefinitions;
    FOR_LIST(it, structDefinitions)
    {
        const GPUStructDefinition& structDefinition = *it;
        get().structType(structDefinition);
    }
    
    const std::vector<GPUVariableDefinitionData>& consts = material->getMaterialShaderVariables().mConsts;
    FOR_LIST(it, consts)
    {
        const GPUVariableDefinitionData& constVar = *it;
        get().attribute(constVar);
    }

    const std::vector<GPUVertexBuffer>& gpuVertexBuffers = gpuVertexBuffersLayout.getBuffers();
    FOR_LIST(it, gpuVertexBuffers)
    {
        const GPUVertexBuffer& gpuVertexBuffer = *it;
        get().attribute({gpuVertexBuffer.mData.mGPUVariableData, gpuVertexBuffer.getAttributeLocation()});
    }

    const std::vector<GPUVariableDefinitionData>& uniforms = material->getMaterialShaderVariables().mUniforms;
    FOR_LIST(it, uniforms)
    {
        const GPUVariableDefinitionData& uniformVar = *it;
        get().attribute(uniformVar);
    }

    const std::vector<GPUSharedBufferData>& sharedBuffers = material->getMaterialShaderVariables().mSharedBuffers;
    FOR_LIST(it, sharedBuffers)
    {
        const GPUSharedBufferData& block = *it;
        get().attributeBlock(block);
    }

    const std::vector<GPUVariableDefinitionData>& outputs = material->getMaterialShaderVariables().mVertexOutputs;
    FOR_LIST(it, outputs)
    {
        const GPUVariableDefinitionData& outputVar = *it;
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
    auto& color = get().getAttribute(GPUBuiltIn::VertexInput::mColor.mName);
    auto& normal = get().getAttribute(GPUBuiltIn::VertexInput::mNormal.mName);
    auto& textureCoord = get().getAttribute(GPUBuiltIn::VertexInput::mTextureCoord.mName);
    auto& instanceId = get().getAttribute(GPUBuiltIn::VertexInput::mInstanceID.mName);
    
    auto& globalMatricesBlock = get().getAttributeBlock(GPUBuiltIn::SharedBuffers::mGlobalMatrices.mInstanceName);    
    Variable projectionMatrix(globalMatricesBlock.mGPUSharedBufferData.getScopedGPUVariableData(0));
    Variable viewMatrix(globalMatricesBlock.mGPUSharedBufferData.getScopedGPUVariableData(1));

    auto& modelMatricesBlock = get().getAttributeBlock(GPUBuiltIn::SharedBuffers::mModelMatrices.mInstanceName);    
    Variable modelMatrices(modelMatricesBlock.mGPUSharedBufferData.getScopedGPUVariableData(0));

    if(material->getMaterialData().mReceiveLight)
    {
        auto& ligthsBlock = get().getAttributeBlock(GPUBuiltIn::SharedBuffers::mLights.mInstanceName);    
        Variable lights(ligthsBlock.mGPUSharedBufferData.getScopedGPUVariableData(0));
    }
    
    auto& outColor = get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    auto& outTextureCoord = get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord.mName);
    auto& outNormal = get().getAttribute(GPUBuiltIn::VertexOutput::mNormal.mName);
    auto& fragPosition = get().getAttribute(GPUBuiltIn::VertexOutput::mFragPosition.mName);

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
    set(finalPositon, modelMatrices.at(instanceId).mul(finalPositon)).
    set(GPUBuiltIn::VertexOutput::mPosition, PVMatrix.mul(finalPositon)).
    set(outTextureCoord, textureCoord).
    set(outNormal, call("mat3", {call("transpose", {call("inverse", {modelMatrices.at(instanceId)})})}).mul(normal)).
    set(fragPosition, call("vec3", {finalPositon}));

    if(material->getMaterialData().mUseVertexColor)
    {
       mainFunc.body().
       set(outColor, color);
    }
}

void ShaderBuilder::createFragmentShader(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material)
{
    using namespace ShaderBuilderNodes;
    using namespace ShaderBuilderNodes::Expressions;
    
    const std::vector<GPUStructDefinition>& structDefinitions = material->getMaterialShaderVariables().mStructDefinitions;
    FOR_LIST(it, structDefinitions)
    {
        const GPUStructDefinition& structDefinition = *it;
        get().structType(structDefinition);
    }
    
    const std::vector<GPUVariableDefinitionData>& consts = material->getMaterialShaderVariables().mConsts;
    FOR_LIST(it, consts)
    {
        const GPUVariableDefinitionData& constVar = *it;
        get().attribute(constVar);
    }

    const std::vector<GPUVariableDefinitionData>& uniforms = material->getMaterialShaderVariables().mUniforms;
    FOR_LIST(it, uniforms)
    {
        const GPUVariableDefinitionData& uniformVar = *it;
        get().attribute(uniformVar);
    }

    const std::vector<GPUSharedBufferData>& sharedBuffers = material->getMaterialShaderVariables().mSharedBuffers;
    FOR_LIST(it, sharedBuffers)
    {
        const GPUSharedBufferData& block = *it;
        get().attributeBlock(block);
    }

    const std::vector<GPUVariableDefinitionData>& fragmentInputs = material->getMaterialShaderVariables().mFragmentInputs;
    FOR_LIST(it, fragmentInputs)
    {
        const GPUVariableDefinitionData& inputVar = *it;
        get().attribute(inputVar);
    }

    const std::vector<GPUVariableDefinitionData>& fragmentOutputs = material->getMaterialShaderVariables().mFragmentOutputs;
    FOR_LIST(it, fragmentOutputs)
    {
        const GPUVariableDefinitionData& outputVar = *it;
        get().attribute(outputVar);
    }

    auto& inColor = get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    auto& inTextureCoord = get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord.mName);
    auto& inNormal = get().getAttribute(GPUBuiltIn::VertexOutput::mNormal.mName);
    auto& inPosition = get().getAttribute(GPUBuiltIn::VertexOutput::mFragPosition.mName);
    auto& outColor = get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);

    auto& sampler = get().getAttribute(GPUBuiltIn::Uniforms::mSampler.mName);
    auto& baseColor = get().getAttribute(GPUBuiltIn::Uniforms::mBaseColor.mName);

    auto& mainFunc = get().function("void", "main");

    Variable t;
    Variable color;
    Variable ambient;
    Variable diffuse;
    Variable diffuseFinal;

    mainFunc.body().
    variable(color, "vec4", "color").
    variable(ambient, "vec3", "ambient", call("vec3", {{"0.0"}, {"0.0"}, {"0.0"}})).
    variable(diffuseFinal, "vec3", "diffuseFinal", call("vec3", {{"0.0"}, {"0.0"}, {"0.0"}}));

    if(material->getMaterialData().mReceiveLight)
    {
        auto& ligthsBlock = get().getAttributeBlock(GPUBuiltIn::SharedBuffers::mLights.mInstanceName);    
        Variable lights(ligthsBlock.mGPUSharedBufferData.getScopedGPUVariableData(0));

        Variable norm;
        Variable lightDir;
        Variable diff;
        Variable diffuse;

        mainFunc.body().
        set(ambient, call("vec3", {{"0.0"}, {"0.1"}, {"0.0"}})).
        variable(norm, "vec3", "norm", call("normalize", {inNormal})).
        variable(lightDir, "vec3", "lightDir", call("normalize", {call("vec3",{{"400"},{"0"},{"300"}}).sub(inPosition)})).
        variable(diff, "float", "diff", call("max", {call("dot", {norm, lightDir}), {"-1"}})).
        variable(diffuse, "vec3", "diffuse", diff.mul(call("vec3", {{"0.8"}, {"0.8"}, {"0.8"}})));

        mainFunc.body().
        set(diffuseFinal, diffuse);

        // vec3 norm = normalize(Normal);
        // vec3 lightDir = normalize(light.position - FragPos);
        // float diff = max(dot(norm, lightDir), 0.0);
        // vec3 diffuse = light.diffuse * (diff * material.diffuse);
    }

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
    set(outColor, color);

    if(material->hasTexture())
    {
        mainFunc.body().
        set(outColor, call("texture2D", {sampler, inTextureCoord}));

        if(material->getMaterialData().mUseColorAsTint)
        {
            mainFunc.body().
            set(outColor.dot("r"), outColor.dot("r").add(color.dot("r"))).
            set(outColor.dot("g"), outColor.dot("g").add(color.dot("g"))).
            set(outColor.dot("b"), outColor.dot("b").add(color.dot("b"))).
            set(outColor.dot("a"), outColor.dot("a").add(color.dot("a")));
        }
    }

    mainFunc.body().
    set(outColor, outColor.add(call("vec4", {diffuseFinal.add(ambient), {"1"}})));
    
    if(material->getMaterialData().mAlphaEnabled)
    {
        mainFunc.body().
        ifBlock(outColor.dot("r").add(outColor.dot("g").add(outColor.dot("b"))).eq({"0"})).
            line("discard").
        end();
    }
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