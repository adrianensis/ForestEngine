#include "Graphics/Material/Shader/ShaderDefault.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Light/Light.hpp"
using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

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

    auto& modelMatricesBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices.mInstanceName);
    if(modelMatricesBuffer.isValid())
    {
        Variable modelMatrices;
        modelMatrices = Variable(modelMatricesBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
        auto& objectId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mObjectID);
        if(objectId.isValid())
        {
            shaderBuilder.getMain().set(finalPositon, modelMatrices.at(objectId).mul(finalPositon));
        }
    }

    shaderBuilder.setVariableInCache(finalPositon);
    
    vertexShaderCalculatePositionOutputCustom(shaderBuilder);

    auto& globalDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable projectionViewMatrix(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));

    shaderBuilder.getMain().
    set(GPUBuiltIn::VertexOutput::mPosition, projectionViewMatrix.mul(finalPositon));

    auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mFragPosition);
    shaderBuilder.getMain().set(fragPosition, call(GPUBuiltIn::PrimitiveTypes::mVector3, {finalPositon}));

    if(getShaderData().mMaterial->getMaterialData().mReceiveShadows)
    {
        auto& shadowMappingBuffer = shaderBuilder.get().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData.mInstanceName);    
        Variable lightProjectionViewMatrix(shadowMappingBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));

        auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mFragPosition);
        auto& fragPositionLight = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mFragPositionLight);
        shaderBuilder.getMain().set(fragPositionLight, lightProjectionViewMatrix.mul(call(GPUBuiltIn::PrimitiveTypes::mVector4, {fragPosition, {"1"}})));
    }
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
    auto& modelMatricesBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices.mInstanceName);
    if(modelMatricesBuffer.isValid())
    {
        Variable modelMatrices;
        modelMatrices = Variable(modelMatricesBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
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
            // set(outNormal, call("mat3", {call("transpose", {call("inverse", {modelMatrices.at(objectId)})})}).mul(finalNormal));
            shaderBuilder.getMain().
            set(outNormal, call(GPUBuiltIn::PrimitiveTypes::mMatrix3, {modelMatrices.at(objectId)}).mul(finalNormal));
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
    auto& outTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoords.at(0));
    shaderBuilder.getMain().
    set(outTextureCoord, textureCoord);
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

    if(getShaderData().mMaterial->getShader()->hasTexture(TextureBindingNames::smBaseColor))
    {
        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoords.at(0));
        auto& sampler = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getSampler(TextureBindingNames::smBaseColor));
        shaderBuilder.getMain().
        set(outColor, call("texture", {sampler, inTextureCoord}));
    }
}

void ShaderDefault::generateShaderBuilderData(ShaderDefault::ShaderBuilderData& shaderBuilderData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    if(getShaderData().mMaterial->getMaterialData().mIsFont)
    {
        HashedString samplerName = TextureBindingNames::smBaseColor;
        shaderBuilderData.mFragmentVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getSampler(samplerName));
    }
    else
    {
        FOR_MAP(it, getShaderData().mMaterial->getMaterialData().mTextureBindings)
        {
            CHECK_MSG(!it->second.mPath.get().empty(), "texture mPath cannot be empty!");

            HashedString samplerName = it->first;
            switch (it->second.mStage)
            {
                case GPUPipelineStage::VERTEX:
                    shaderBuilderData.mVertexVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getSampler(samplerName));
                break;
                case GPUPipelineStage::FRAGMENT:
                    shaderBuilderData.mFragmentVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getSampler(samplerName));
                break;

                default:
                    CHECK_MSG(false, "Invalid Stage for texture binding!");
            }
        }

        FOR_MAP(it, getShaderData().mMaterial->getShader()->getShaderData().mFramebufferBindings)
        {
            CHECK_MSG(!it->second.mSamplerName.get().empty(), "frambuffer texture samplerName cannot be empty!");

            HashedString samplerName = it->second.mSamplerName;
            switch (it->second.mStage)
            {
                case GPUPipelineStage::VERTEX:
                    shaderBuilderData.mVertexVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getSampler(samplerName));
                break;
                case GPUPipelineStage::FRAGMENT:
                    shaderBuilderData.mFragmentVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getSampler(samplerName));
                break;

                default:
                    CHECK_MSG(false, "Invalid Stage for frambuffer texture binding!");
            }
        }
    }

    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(getShaderData().mPropertiesBlockStructDefinition);

    FOR_LIST(it, gpuSharedBuffersContainer.getSharedBuffers())
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(it->getGPUSharedBufferData());
    }

    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mGlobalData);
    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mModelMatrices);
    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(getShaderData().mPropertiesBlockSharedBufferData);

    shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mPI);
    shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mPI180);
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mBonesIDs))
    {
        shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mMaxBones);
        shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mMaxBoneInfluence);
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mBonesMatrices);
    }

    FOR_LIST(it, gpuVertexBuffersContainer.getVertexBuffers())
    {
        shaderBuilderData.mVertexVariables.mVertexInputs.push_back(*it);
    }

    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mTextureCoords.at(0));
    
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mColor))
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mColor);
    }
    
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mNormal))
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mNormal);
    }
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mFragPosition);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mFragPositionLight);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mInstanceID);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mObjectID);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mMaterialInstanceID);
    
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mTextureCoords.at(0));
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mColor);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mNormal))
    {
        shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mNormal);
    }
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mFragPosition);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mFragPositionLight);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mInstanceID);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mObjectID);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mMaterialInstanceID);
    shaderBuilderData.mFragmentVariables.mFragmentOutputs.push_back(GPUBuiltIn::FragmentOutput::mColor);

    if(getShaderData().mMaterial->getMaterialData().mReceiveLight)
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(LightBuiltIn::mLightsBufferData);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mDirectionalLightStructDefinition);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mPointLightStructDefinition);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mSpotLightStructDefinition);
    }

    if(getShaderData().mMaterial->getMaterialData().mReceiveShadows)
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(LightBuiltIn::mShadowMappingBufferData);
    }
}

void ShaderDefault::registerVertexShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    ShaderBuilderData shaderBuilderData;
    generateShaderBuilderData(shaderBuilderData, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mStructDefinitions) { shaderBuilder.get().structType(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mVertexVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mVertexVariables.mVertexInputs) { shaderBuilder.get().attribute({it->mData.mGPUVariableData, it->getAttributeLocation()}); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mVertexVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mSharedBuffers) { shaderBuilder.get().sharedBuffer(*it); }
    FOR_LIST(it, shaderBuilderData.mVertexVariables.mVertexOutputs) { shaderBuilder.get().attribute(*it); }

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mBonesIDs))
    {
        registerFunctionCalculateBoneTransform(shaderBuilder);
    }
}

void ShaderDefault::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    ShaderBuilderData shaderBuilderData;
    generateShaderBuilderData(shaderBuilderData, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mStructDefinitions) { shaderBuilder.get().structType(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mSharedBuffers) { shaderBuilder.get().sharedBuffer(*it); }
    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mFragmentInputs) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mFragmentOutputs) { shaderBuilder.get().attribute(*it); }
}

void ShaderDefault::createVertexShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    registerVertexShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mBonesIDs))
    {
        vertexShaderCalculateBoneMatrix(shaderBuilder);
    }

    vertexShaderCalculatePositionOutput(shaderBuilder);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mNormal))
    {
        vertexShaderCalculateNormalOutput(shaderBuilder);
    }

    if(getShaderData().mMaterial->getShader()->hasTexture(TextureBindingNames::smBaseColor))
    {
        vertexShaderCalculateTextureCoordinateOutput(shaderBuilder);
    }

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mColor))
    {
        vertexShaderCalculateVertexColorOutput(shaderBuilder);
    }

    vertexShaderCalculateInstanceIdOutput(shaderBuilder);
}

void ShaderDefault::createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    
    fragmentShaderCode(shaderBuilder);
}

void ShaderDefault::registerFunctionCalculateBoneTransform(ShaderBuilder& shaderBuilder) const
{
    FunctionDefinition func(GPUBuiltIn::Functions::mCalculateBoneTransform);
    
    auto& bonesIDs = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mBonesIDs);
    auto& bonesWeights = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mBonesWeights);
    auto& MAX_BONES = shaderBuilder.get().getAttribute(GPUBuiltIn::Consts::mMaxBones);
    auto& MAX_BONE_INFLUENCE = shaderBuilder.get().getAttribute(GPUBuiltIn::Consts::mMaxBoneInfluence);
    auto& bonesMatricesblock = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices.mInstanceName);    
    Variable bonesTransform(bonesMatricesblock.mGPUSharedBufferData.getScopedGPUVariableData(0));
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