#include "Graphics/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/GPU/GPUVertexBuffersLayout.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderFunctionsLibrary.hpp"

using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

void ShaderBuilder::createVertexShader(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material)
{
    registerVertexShaderData(gpuVertexBuffersLayout, material);

    // retrieve the needed attributes
    auto& position = get().getAttribute(GPUBuiltIn::VertexInput::mPosition.mName);
    auto& color = get().getAttribute(GPUBuiltIn::VertexInput::mColor.mName);
    auto& normal = get().getAttribute(GPUBuiltIn::VertexInput::mNormal.mName);
    auto& textureCoord = get().getAttribute(GPUBuiltIn::VertexInput::mTextureCoord.mName);
    auto& instanceId = get().getAttribute(GPUBuiltIn::VertexInput::mInstanceID.mName);
    
    auto& globalDataBuffer = get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable projectionMatrix(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
    Variable viewMatrix(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(1));

    auto& modelMatricesBuffer = get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices.mInstanceName);
    Variable modelMatrices;
    if(material->getMaterialData().mUseModelMatrix)
    {
        modelMatrices = Variable(modelMatricesBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
    }

    if(material->getMaterialData().mReceiveLight)
    {
        auto& ligthsDataBuffer = get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mLightsData.mInstanceName);    
        Variable lights(ligthsDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
    }
    
    auto& outColor = get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    auto& outTextureCoord = get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord.mName);
    auto& outNormal = get().getAttribute(GPUBuiltIn::VertexOutput::mNormal.mName);
    auto& fragPosition = get().getAttribute(GPUBuiltIn::VertexOutput::mFragPosition.mName);

    auto& mainFunc = get().function(GPUBuiltIn::Functions::mMain);

    Variable finalPositon;
    Variable finalNormal;
    Variable PVMatrix;

    mainFunc.body().
    variable(finalPositon, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "finalPositon", call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {position, {"1.0f"}}));

    if(material->getMaterialData().mUseNormals)
    {
        mainFunc.body().
        variable(finalNormal, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "finalNormal", call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {normal}));
    }
    
    if(material->getMaterialData().mIsSkinned)
    {
        Variable boneMatrix;
        mainFunc.body().
        variable(boneMatrix, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "boneMatrix", call(GPUBuiltIn::Functions::mCalculateBoneTransform.mName, {})).
        set(finalPositon, boneMatrix.mul(finalPositon));
        
        if(material->getMaterialData().mUseNormals)
        {
            Variable transformedNormal;
            mainFunc.body().
            variable(transformedNormal, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "transformedNormal", boneMatrix.mul(call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {finalNormal, {"1.0f"}}))).
            set(finalNormal, transformedNormal.dot("xyz"));
        }
    }

    mainFunc.body().variable(PVMatrix, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "PV_Matrix", projectionMatrix.mul(viewMatrix));

    if(material->getMaterialData().mUseModelMatrix)
    {
        mainFunc.body().set(finalPositon, modelMatrices.at(instanceId).mul(finalPositon));
    }

    mainFunc.body().
    set(GPUBuiltIn::VertexOutput::mPosition, PVMatrix.mul(finalPositon));

    if(material->hasTexture())
    {
        mainFunc.body().
        set(outTextureCoord, textureCoord);
    }
    
    if(material->getMaterialData().mUseNormals)
    {
        // if(material->getMaterialData().mUseModelMatrix)
        // {
        //     /*
        //         - NOTE - 
        //         There are many sources online that tell you that you need the transpose of the inverse of the world matrix in order to
        //         transform the normal vector. This is correct, however, we usually don't need to go that far. Our world matrices are
        //         always orthogonal (their vectors are always orthogonal). Since the inverse of an orthogonal matrix is equal to its transpose,
        //         the transpose of the inverse is actually the transpose of the transpose, so we end up with the original matrix.
        //         As long as we avoid doing distortions (scaling one axis differently than the rest) we are fine with the approach I presented above. 
            
        //     */
        //     mainFunc.body().
        //     set(outNormal, call("mat3", {call("transpose", {call("inverse", {modelMatrices.at(instanceId)})})}).mul(finalNormal));
        // }
        // else
        // {
            mainFunc.body().
            set(outNormal, finalNormal);
        // }
    }

    mainFunc.body().set(fragPosition, call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {finalPositon}));

    if(material->getMaterialData().mUseVertexColor)
    {
       mainFunc.body().
       set(outColor, color);
    }
}

void ShaderBuilder::createFragmentShader(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material)
{
    registerFragmentShaderData(gpuVertexBuffersLayout, material);

    auto& inColor = get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    auto& inTextureCoord = get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord.mName);
    auto& outColor = get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);

    auto& sampler = get().getAttribute(GPUBuiltIn::Uniforms::mSampler.mName);
    auto& baseColor = get().getAttribute(GPUBuiltIn::Uniforms::mBaseColor.mName);

    auto& mainFunc = get().function(GPUBuiltIn::Functions::mMain);

    Variable color;
    mainFunc.body().
    variable(color, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "color", call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {{"0.0"}, {"0.0"}, {"0.0"}, {"1.0"}}));

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

    if(material->getMaterialData().mReceiveLight && material->getMaterialData().mUseNormals)
    {
        Variable phong;
        mainFunc.body().
        variable(phong, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "phong", call(GPUBuiltIn::Functions::mCalculatePhong.mName, {})).
        set(outColor, outColor.mul(phong));
    }
    
    if(material->getMaterialData().mAlphaEnabled)
    {
        mainFunc.body().
        ifBlock(outColor.dot("r").add(outColor.dot("g").add(outColor.dot("b"))).eq({"0"})).
            line("discard").
        end();
    }
}

ShaderBuilder::ShaderBuilderData ShaderBuilder::generateShaderBuilderData(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material) const
{
    ShaderBuilderData shaderBuilderData;
    
    shaderBuilderData.mCommonVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::mTime);
    shaderBuilderData.mCommonVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::mWindowSize);
    shaderBuilderData.mCommonVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::mBaseColor);
    shaderBuilderData.mCommonVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::mSampler);

    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mGlobalData);

    if(material->getMaterialData().mUseModelMatrix)
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mModelMatrices);
    }

    if(material->getMaterialData().mReceiveLight)
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mLightsData);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(GPUBuiltIn::StructDefinitions::mLight);
    }

    if(material->getMaterialData().mIsSkinned)
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mBonesMatrices);
        shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::MAX_BONES);
        shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::MAX_BONE_INFLUENCE);
    }

    const std::vector<GPUVertexBuffer>& gpuVertexBuffers = gpuVertexBuffersLayout.getBuffers();
    FOR_LIST(it, gpuVertexBuffers)
    {
        const GPUVertexBuffer& gpuVertexBuffer = *it;
        shaderBuilderData.mVertexVariables.mVertexInputs.push_back(gpuVertexBuffer);
    }

    if(material->hasTexture())
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mTextureCoord);
    }
    
    if(material->getMaterialData().mUseVertexColor)
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mColor);
    }
    
    if(material->getMaterialData().mUseNormals)
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mNormal);
    }
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mFragPosition);
    
    if(material->hasTexture())
    {
        shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mTextureCoord);
    }

    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mColor);

    if(material->getMaterialData().mUseNormals)
    {
        shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mNormal);
    }
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mFragPosition);

    shaderBuilderData.mFragmentVariables.mFragmentOutputs.push_back(GPUBuiltIn::FragmentOutput::mColor);

    return shaderBuilderData;
}

void ShaderBuilder::registerVertexShaderData(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material)
{
    ShaderBuilderData shaderBuilderData = generateShaderBuilderData(gpuVertexBuffersLayout, material);

    FOR_LIST(it, shaderBuilderData.mCommonVariables.mStructDefinitions)
    {
        get().structType(*it);
    }
    
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mConsts)
    {
        get().attribute(*it);
    }

    FOR_LIST(it, shaderBuilderData.mVertexVariables.mVertexInputs)
    {
        const GPUVertexBuffer& inputVar = *it;
        get().attribute({inputVar.mData.mGPUVariableData, inputVar.getAttributeLocation()});
    }

    FOR_LIST(it, shaderBuilderData.mCommonVariables.mUniforms)
    {
        get().attribute(*it);
    }

    FOR_LIST(it, shaderBuilderData.mCommonVariables.mSharedBuffers)
    {
        get().sharedBuffer(*it);
    }

    FOR_LIST(it, shaderBuilderData.mVertexVariables.mVertexOutputs)
    {
        get().attribute(*it);
    }

    ShaderBuilderFunctionsLibrary shaderBuilderFunctionsLibrary;
    shaderBuilderFunctionsLibrary.init(get(), material);
    if(material->getMaterialData().mIsSkinned)
    {
        // get().function(shaderBuilderFunctionsLibrary.mFunctions.at(GPUBuiltIn::Functions::mCalculateSkinnedPosition.mName));
        get().function(shaderBuilderFunctionsLibrary.mFunctions.at(GPUBuiltIn::Functions::mCalculateBoneTransform.mName));
    }
}

void ShaderBuilder::registerFragmentShaderData(const GPUVertexBuffersLayout& gpuVertexBuffersLayout, Ptr<const Material> material)
{
    ShaderBuilderData shaderBuilderData = generateShaderBuilderData(gpuVertexBuffersLayout, material);
    
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mStructDefinitions)
    {
        get().structType(*it);
    }
    
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mConsts)
    {
        get().attribute(*it);
    }

    FOR_LIST(it, shaderBuilderData.mCommonVariables.mUniforms)
    {
        get().attribute(*it);
    }

    FOR_LIST(it, shaderBuilderData.mCommonVariables.mSharedBuffers)
    {
        get().sharedBuffer(*it);
    }

    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mFragmentInputs)
    {
        get().attribute(*it);
    }

    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mFragmentOutputs)
    {
        get().attribute(*it);
    }

    ShaderBuilderFunctionsLibrary shaderBuilderFunctionsLibrary;
    shaderBuilderFunctionsLibrary.init(get(), material);
    if(material->getMaterialData().mReceiveLight)
    {
        get().function(shaderBuilderFunctionsLibrary.mFunctions.at(GPUBuiltIn::Functions::mCalculatePhong.mName));
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