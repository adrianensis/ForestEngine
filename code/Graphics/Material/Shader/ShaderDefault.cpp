#include "Graphics/Material/Shader/ShaderDefault.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Light/Light.hpp"
using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

void ShaderDefault::registerTextures()
{
    mShaderData.mTextures.insert(TextureBindingNames::smBaseColor);
}

void ShaderDefault::vertexShaderCalculateBoneMatrix(ShaderBuilder& shaderBuilder) const
{
    Variable boneMatrix;
    shaderBuilder.getMain().
    variable(boneMatrix, GPUBuiltIn::PrimitiveTypes::mMatrix4, "boneMatrix", call(GPUBuiltIn::Functions::mCalculateBoneTransform, {}));

    shaderBuilder.setVariableInCache(boneMatrix);
}

void ShaderDefault::vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const
{
    auto& position = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mPosition);

    Variable finalPositon;
    shaderBuilder.getMain().
    variable(finalPositon, GPUBuiltIn::PrimitiveTypes::mVector4, "finalPositon", call(GPUBuiltIn::PrimitiveTypes::mVector4, {position, {"1.0f"}}));
    
    auto& bonesIDs = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mBonesIDs);
    if(bonesIDs.isValid())
    {
        Variable boneMatrix = shaderBuilder.getVariableFromCache("boneMatrix");
        shaderBuilder.getMain().
        set(finalPositon, boneMatrix.mul(finalPositon));
    }

    auto& modelMatricesBuffer = shaderBuilder.get().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices.mInstanceName);
    if(modelMatricesBuffer.isValid())
    {
        Variable modelMatrices;
        modelMatrices = Variable(modelMatricesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        auto& objectId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mObjectID);
        if(objectId.isValid())
        {
            shaderBuilder.getMain().set(finalPositon, modelMatrices.at(objectId).mul(finalPositon));
        }
    }

    shaderBuilder.setVariableInCache(finalPositon);
    
    vertexShaderCalculatePositionOutputCustom(shaderBuilder);

    auto& globalDataBuffer = shaderBuilder.get().getUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData.mInstanceName);    
    Variable projectionViewMatrix(globalDataBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));

    shaderBuilder.getMain().
    set(GPUBuiltIn::VertexOutput::mPosition, projectionViewMatrix.mul(finalPositon));

    auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mFragPosition);
    shaderBuilder.getMain().set(fragPosition, call(GPUBuiltIn::PrimitiveTypes::mVector3, {finalPositon}));
}

void ShaderDefault::vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const
{

}

void ShaderDefault::vertexShaderCalculateNormalOutput(ShaderBuilder& shaderBuilder) const
{
    auto& normal = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mNormal);

    Variable finalNormal;
    shaderBuilder.getMain().
    variable(finalNormal, GPUBuiltIn::PrimitiveTypes::mVector3, "finalNormal", call(GPUBuiltIn::PrimitiveTypes::mVector3, {normal}));

    auto& bonesIDs = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mBonesIDs);
    if(bonesIDs.isValid())
    {
        Variable boneMatrix = shaderBuilder.getVariableFromCache("boneMatrix");
        Variable transformedNormal;
        shaderBuilder.getMain().
        variable(transformedNormal, GPUBuiltIn::PrimitiveTypes::mVector4, "transformedNormal", boneMatrix.mul(call(GPUBuiltIn::PrimitiveTypes::mVector4, {finalNormal, {"1.0f"}}))).
        set(finalNormal, transformedNormal.dot("xyz"));
    }

    auto& outNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mNormal);
    auto& modelMatricesBuffer = shaderBuilder.get().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices.mInstanceName);
    if(modelMatricesBuffer.isValid())
    {
        Variable modelMatrices;
        modelMatrices = Variable(modelMatricesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        auto& objectId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mObjectID);
        if(objectId.isValid())
        {
            /*
                - NOTE - 
                There are many sources online that tell you that you need the transpose of the inverse of the world matrix in order to
                transform the normal vector. This is correct, however, we usually don't need to go that far. Our world matrices are
                always orthogonal (their vectors are always orthogonal). Since the inverse of an orthogonal matrix is equal to its transpose,
                the transpose of the inverse is actually the transpose of the transpose, so we end up with the original matrix.
                As long as we avoid doing distortions (scaling one axis differently than the rest) we are fine with the approach I presented above. 
            */
            // shaderBuilder.getMain().
            shaderBuilder.getMain().
            set(outNormal, call("mat3", {call("transpose", {call("inverse", {modelMatrices.at(objectId)})})}).mul(finalNormal));
            // set(outNormal, call(GPUBuiltIn::PrimitiveTypes::mMatrix3, {modelMatrices.at(objectId)}).mul(finalNormal));
        }
    }
    else
    {
        shaderBuilder.getMain().
        set(outNormal, finalNormal);
    }
}

void ShaderDefault::vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const
{   
    auto& textureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mTextureCoords.at(0));
    if(textureCoord.isValid())
    {
        auto& outTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoords.at(0));
        shaderBuilder.getMain().
        set(outTextureCoord, textureCoord);
    }
}

void ShaderDefault::vertexShaderCalculateVertexColorOutput(ShaderBuilder& shaderBuilder) const
{
    auto& color = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mColor);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mColor);
    shaderBuilder.getMain().
    set(outColor, color);
}

void ShaderDefault::vertexShaderCalculateInstanceIdOutput(ShaderBuilder& shaderBuilder) const
{
    auto& instanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mInstanceID);
    auto& objectId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mObjectID);
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mMaterialInstanceID);
    auto& outInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mInstanceID);
    auto& outObjectId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mObjectID);
    auto& outMaterialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID);

    if(instanceId.isValid())
    {
        shaderBuilder.getMain().
        set(outInstanceId, instanceId);
    }
    if(objectId.isValid())
    {
        shaderBuilder.getMain().
        set(outObjectId, objectId);
    }
    if(materialInstanceId.isValid())
    {
        shaderBuilder.getMain().
        set(outMaterialInstanceId, materialInstanceId);
    }
}

void ShaderDefault::fragmentShaderCode(ShaderBuilder& shaderBuilder) const
{
    auto& inColor = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mColor);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor);
    
    Variable baseColor;
    shaderBuilder.getMain().
    variable(baseColor, GPUBuiltIn::PrimitiveTypes::mVector4, "baseColor", call(GPUBuiltIn::PrimitiveTypes::mVector4, {{"0.0"}, {"0.0"}, {"0.0"}, {"1.0"}}));

    if(inColor.isValid())
    {
        shaderBuilder.getMain().
        set(baseColor, inColor);
    }

    shaderBuilder.getMain().
    set(outColor, baseColor);

    auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mTextureCoords.at(0));
    if(inTextureCoord.isValid())
    {
        auto& textureHandler = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getTextureHandler(TextureBindingNames::smBaseColor));
        auto& texturesBuffer = shaderBuilder.get().getUniformBuffer(GPUBuiltIn::UniformBuffers::mTextures.mInstanceName);    
        Variable textures(texturesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        shaderBuilder.getMain().
        ifBlock(textureHandler.notEq("0"s)).
            set(outColor, call("texture", {textures.at(textureHandler), inTextureCoord})).
        end();
    }
}

void ShaderDefault::generateGPUProgramData(GPUProgramData& gpuProgramData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
    FOR_MAP(it, mShaderData.mTextures)
    {
        gpuProgramData.mFragmentVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getTextureHandler(*it));
    }
    
    // FOR_MAP(it, getShaderData().mMaterial->getMaterialData().mTextureBindings)
    // {
    //     CHECK_MSG(!it->second.mPath.get().empty(), "texture mPath cannot be empty!");

    //     HashedString samplerName = it->first;
    //     switch (it->second.mStage)
    //     {
    //         // case GPUPipelineStage::VERTEX:
    //         // {
    //         //     gpuProgramData.mVertexVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getTextureHandler(samplerName));
    //         // }
    //         // break;
    //         case GPUPipelineStage::FRAGMENT:
    //         {
    //             gpuProgramData.mFragmentVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getTextureHandler(samplerName));
    //         }
    //         break;

    //         default:
    //             CHECK_MSG(false, "Invalid Stage for texture binding!");
    //     }
    // }

    FOR_MAP(it, getShaderData().mFramebufferBindings)
    {
        CHECK_MSG(!it->second.mSamplerName.get().empty(), "frambuffer texture samplerName cannot be empty!");

        HashedString samplerName = it->second.mSamplerName;
        switch (it->second.mStage)
        {
            // case GPUPipelineStage::VERTEX:
            //     gpuProgramData.mVertexVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getSampler(samplerName));
            // break;
            case GPUPipelineStage::FRAGMENT:
                gpuProgramData.mFragmentVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getSampler(samplerName));
            break;

            default:
                CHECK_MSG(false, "Invalid Stage for frambuffer texture binding!");
        }
    }

    gpuProgramData.mCommonVariables.mStructDefinitions.push_back(getShaderData().mPropertiesBlockStructDefinition);

    gpuProgramData.mCommonVariables.mUniformBuffers.push_back(GPUBuiltIn::UniformBuffers::mGlobalData);
    gpuProgramData.mCommonVariables.mUniformBuffers.push_back(GPUBuiltIn::UniformBuffers::mTextures);
    gpuProgramData.mCommonVariables.mUniformBuffers.push_back(GPUBuiltIn::UniformBuffers::mModelMatrices);
    gpuProgramData.mCommonVariables.mUniformBuffers.push_back(getShaderData().mPropertiesBlockUniformBufferData);

    gpuProgramData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mPI);
    gpuProgramData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mPI180);
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mBonesIDs))
    {
        gpuProgramData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mMaxBones);
        gpuProgramData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mMaxBoneInfluence);
        gpuProgramData.mCommonVariables.mUniformBuffers.push_back(GPUBuiltIn::UniformBuffers::mBonesMatrices);
    }

    FOR_LIST(it, gpuVertexBuffersContainer.getVertexBuffers())
    {
        gpuProgramData.mVertexVariables.mVertexInputs.push_back(*it);
    }

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mTextureCoords.at(0)))
    {
        gpuProgramData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mTextureCoords.at(0));
    }
    
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mColor))
    {
        gpuProgramData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mColor);
    }
    
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mNormal))
    {
        gpuProgramData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mNormal);
    }

    gpuProgramData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mFragPosition);
    gpuProgramData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mFragPositionLight);
    gpuProgramData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mInstanceID);
    gpuProgramData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mObjectID);
    gpuProgramData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mMaterialInstanceID);
    
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mTextureCoords.at(0)))
    {
        gpuProgramData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mTextureCoords.at(0));
    }

    gpuProgramData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mColor);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mNormal))
    {
        gpuProgramData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mNormal);
    }
    gpuProgramData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mFragPosition);
    gpuProgramData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mFragPositionLight);
    gpuProgramData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mInstanceID);
    gpuProgramData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mObjectID);
    gpuProgramData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mMaterialInstanceID);
    gpuProgramData.mFragmentVariables.mFragmentOutputs.push_back(GPUBuiltIn::FragmentOutput::mColor);
}

void ShaderDefault::registerVertexShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
    GPUProgramData gpuProgramData;
    generateGPUProgramData(gpuProgramData, gpuVertexBuffersContainer);
    FOR_LIST(it, gpuProgramData.mCommonVariables.mStructDefinitions) { shaderBuilder.get().structType(*it); }
    FOR_LIST(it, gpuProgramData.mCommonVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, gpuProgramData.mVertexVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, gpuProgramData.mVertexVariables.mVertexInputs) { shaderBuilder.get().attribute({it->mData.mGPUVariableData, it->getAttributeLocation()}); }
    FOR_LIST(it, gpuProgramData.mCommonVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, gpuProgramData.mVertexVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    u32 uniformBufferIndex = 0;
    FOR_LIST(it, gpuProgramData.mCommonVariables.mUniformBuffers) { shaderBuilder.get().uniformBuffer(UniformBuffer(*it, uniformBufferIndex)); uniformBufferIndex++; }
    u32 vertexOutputIndex = 0;
    FOR_LIST(it, gpuProgramData.mVertexVariables.mVertexOutputs) { shaderBuilder.get().attribute(Attribute(*it, vertexOutputIndex)); vertexOutputIndex++; }

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mBonesIDs))
    {
        registerFunctionCalculateBoneTransform(shaderBuilder);
    }
}

void ShaderDefault::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
    GPUProgramData gpuProgramData;
    generateGPUProgramData(gpuProgramData, gpuVertexBuffersContainer);
    FOR_LIST(it, gpuProgramData.mCommonVariables.mStructDefinitions) { shaderBuilder.get().structType(*it); }
    FOR_LIST(it, gpuProgramData.mCommonVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, gpuProgramData.mFragmentVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, gpuProgramData.mCommonVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, gpuProgramData.mFragmentVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    u32 uniformBufferIndex = 0;
    FOR_LIST(it, gpuProgramData.mCommonVariables.mUniformBuffers) { shaderBuilder.get().uniformBuffer(UniformBuffer(*it, uniformBufferIndex)); uniformBufferIndex++; }
    u32 fragmentInputIndex = 0;
    FOR_LIST(it, gpuProgramData.mFragmentVariables.mFragmentInputs) { shaderBuilder.get().attribute(Attribute(*it, fragmentInputIndex)); fragmentInputIndex++; }
    u32 fragmentOutputIndex = 0;
    FOR_LIST(it, gpuProgramData.mFragmentVariables.mFragmentOutputs) { shaderBuilder.get().attribute(Attribute(*it, fragmentOutputIndex)); fragmentOutputIndex++; }
}

void ShaderDefault::createVertexShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
    registerVertexShaderData(shaderBuilder, gpuVertexBuffersContainer);

    // shaderBuilder.get().extension("GL_ARB_bindless_texture");

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mBonesIDs))
    {
        vertexShaderCalculateBoneMatrix(shaderBuilder);
    }

    vertexShaderCalculatePositionOutput(shaderBuilder);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mNormal))
    {
        vertexShaderCalculateNormalOutput(shaderBuilder);
    }

    vertexShaderCalculateTextureCoordinateOutput(shaderBuilder);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mColor))
    {
        vertexShaderCalculateVertexColorOutput(shaderBuilder);
    }

    vertexShaderCalculateInstanceIdOutput(shaderBuilder);
}

void ShaderDefault::createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
    registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer);
    
    // shaderBuilder.get().extension("GL_ARB_bindless_texture");

    fragmentShaderCode(shaderBuilder);
}

void ShaderDefault::registerFunctionCalculateBoneTransform(ShaderBuilder& shaderBuilder) const
{
    FunctionDefinition func(GPUBuiltIn::Functions::mCalculateBoneTransform);
    
    auto& bonesIDs = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mBonesIDs);
    auto& bonesWeights = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mBonesWeights);
    auto& MAX_BONES = shaderBuilder.get().getAttribute(GPUBuiltIn::Consts::mMaxBones);
    auto& MAX_BONE_INFLUENCE = shaderBuilder.get().getAttribute(GPUBuiltIn::Consts::mMaxBoneInfluence);
    auto& bonesMatricesblock = shaderBuilder.get().getUniformBuffer(GPUBuiltIn::UniformBuffers::mBonesMatrices.mInstanceName);    
    Variable bonesTransform(bonesMatricesblock.mGPUUniformBufferData.getScopedGPUVariableData(0));
    Variable currentBoneTransform;
    Variable currentBoneTransformMulWeight;
    Variable finalBoneTransform;
    func.body().
    variable(finalBoneTransform, GPUBuiltIn::PrimitiveTypes::mMatrix4, "finalBoneTransform", call(GPUBuiltIn::PrimitiveTypes::mMatrix4, {{"0.0f"}})).
    forBlock("i", "<", MAX_BONE_INFLUENCE, "++").
        ifBlock(bonesIDs.at("i"), "==", {"-1"}).
            line("continue").
        end().
        ifBlock(bonesIDs.at("i"), ">=", MAX_BONES).
            line("break").
        end().
        variable(currentBoneTransform, GPUBuiltIn::PrimitiveTypes::mMatrix4, "currentBoneTransform", bonesTransform.at(bonesIDs.at("i"))).
        variable(currentBoneTransformMulWeight, GPUBuiltIn::PrimitiveTypes::mMatrix4, "currentBoneTransformMulWeight", currentBoneTransform.mul(bonesWeights.at("i"))).
        set(finalBoneTransform, finalBoneTransform.add(currentBoneTransformMulWeight)).
    end();
    
    func.body().
    ret(finalBoneTransform);

    shaderBuilder.get().function(func);
}