#include "Graphics/Material/MaterialRuntime/MaterialRuntimeDefault.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Light/Light.hpp"
using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

void MaterialRuntimeDefault::vertexShaderCalculateBoneMatrix(ShaderBuilder& shaderBuilder) const
{
    Variable boneMatrix;
    shaderBuilder.getMain().
    variable(boneMatrix, GPUBuiltIn::PrimitiveTypes::mMatrix4, "boneMatrix", call(GPUBuiltIn::Functions::mCalculateBoneTransform, {}));

    shaderBuilder.setVariableInCache(boneMatrix);
}

void MaterialRuntimeDefault::vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const
{
    auto& position = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mPosition);

    Variable finalPositon;
    shaderBuilder.getMain().
    variable(finalPositon, GPUBuiltIn::PrimitiveTypes::mVector4, "finalPositon", call(GPUBuiltIn::PrimitiveTypes::mVector4, {position, {"1.0f"}}));
    
    if(mMaterial->getMaterialData().mIsSkinned)
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

    Variable PVMatrix = shaderBuilder.getVariableFromCache("PV_Matrix");
    shaderBuilder.getMain().
    set(GPUBuiltIn::VertexOutput::mPosition, PVMatrix.mul(finalPositon));

    auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mFragPosition);
    shaderBuilder.getMain().set(fragPosition, call(GPUBuiltIn::PrimitiveTypes::mVector3, {finalPositon}));
}

void MaterialRuntimeDefault::vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const
{

}

void MaterialRuntimeDefault::vertexShaderCalculateNormalOutput(ShaderBuilder& shaderBuilder) const
{
    auto& normal = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mNormal);

    Variable finalNormal;
    shaderBuilder.getMain().
    variable(finalNormal, GPUBuiltIn::PrimitiveTypes::mVector3, "finalNormal", call(GPUBuiltIn::PrimitiveTypes::mVector3, {normal}));

    if(mMaterial->getMaterialData().mIsSkinned)
    {
        Variable boneMatrix = shaderBuilder.getVariableFromCache("boneMatrix");
        Variable transformedNormal;
        shaderBuilder.getMain().
        variable(transformedNormal, GPUBuiltIn::PrimitiveTypes::mVector4, "transformedNormal", boneMatrix.mul(call(GPUBuiltIn::PrimitiveTypes::mVector4, {finalNormal, {"1.0f"}}))).
        set(finalNormal, transformedNormal.dot("xyz"));
    }

    // if(mMaterial->getMaterialData().mUseModelMatrix)
    // {
    //     /*
    //         - NOTE - 
    //         There are many sources online that tell you that you need the transpose of the inverse of the world matrix in order to
    //         transform the normal vector. This is correct, however, we usually don't need to go that far. Our world matrices are
    //         always orthogonal (their vectors are always orthogonal). Since the inverse of an orthogonal matrix is equal to its transpose,
    //         the transpose of the inverse is actually the transpose of the transpose, so we end up with the original matrix.
    //         As long as we avoid doing distortions (scaling one axis differently than the rest) we are fine with the approach I presented above. 
        
    //     */
    //     shaderBuilder.getMain().
    //     set(outNormal, call("mat3", {call("transpose", {call("inverse", {modelMatrices.at(instanceId)})})}).mul(finalNormal));
    // }
    // else
    // {
        auto& outNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mNormal);
        shaderBuilder.getMain().
        set(outNormal, finalNormal);
    // }
}

void MaterialRuntimeDefault::vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const
{

    auto& textureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mTextureCoord);
    auto& outTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord);
    shaderBuilder.getMain().
    set(outTextureCoord, textureCoord);
}

void MaterialRuntimeDefault::vertexShaderCalculateVertexColorOutput(ShaderBuilder& shaderBuilder) const
{
    auto& color = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mColor);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mColor);
    shaderBuilder.getMain().
    set(outColor, color);
}

void MaterialRuntimeDefault::vertexShaderCalculateInstanceIdOutput(ShaderBuilder& shaderBuilder) const
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

void MaterialRuntimeDefault::vertexShaderCalculateProjectionViewMatrix(ShaderBuilder& shaderBuilder) const
{
    auto& globalDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable projectionMatrix(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
    Variable viewMatrix(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(1));
    Variable PVMatrix;

    shaderBuilder.getMain().
    variable(PVMatrix, GPUBuiltIn::PrimitiveTypes::mMatrix4, "PV_Matrix", projectionMatrix.mul(viewMatrix));

    shaderBuilder.setVariableInCache(PVMatrix);
}

void MaterialRuntimeDefault::fragmentShaderCode(ShaderBuilder& shaderBuilder) const
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

    if(mMaterial->hasTexture(TextureMap::BASE_COLOR))
    {
        auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord);
        auto& sampler = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getSampler(EnumsManager::toString<TextureMap>(TextureMap::BASE_COLOR)));
        shaderBuilder.getMain().
        set(outColor, call("texture", {sampler, inTextureCoord}));
    }
}

void MaterialRuntimeDefault::generateShaderBuilderData(MaterialRuntimeDefault::ShaderBuilderData& shaderBuilderData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    if(mMaterial->getMaterialData().mIsFont)
    {
        ConstString samplerName = EnumsManager::toString<TextureMap>(TextureMap::BASE_COLOR);
        shaderBuilderData.mFragmentVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getSampler(samplerName));
    }
    else
    {
        FOR_RANGE(i, 0, mMaterial->getMaterialData().mTextureBindings.size())
        {
            if(!mMaterial->getMaterialData().mTextureBindings[i].mPath.empty())
            {
                ConstString samplerName = EnumsManager::toString<TextureMap>(i);

                switch (mMaterial->getMaterialData().mTextureBindings[i].mStage)
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
        }
    }

    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(getPropertiesBlockStructDefinition());

    FOR_LIST(it, gpuSharedBuffersContainer.getSharedBuffers())
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(it->getGPUSharedBufferData());
    }

    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mGlobalData);
    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mModelMatrices);
    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(getPropertiesBlockSharedBufferData());

    shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mPI);
    shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mPI180);
    if(mMaterial->getMaterialData().mIsSkinned)
    {
        shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mMaxBones);
        shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mMaxBoneInfluence);
    }

    FOR_LIST(it, gpuVertexBuffersContainer.getVertexBuffers())
    {
        shaderBuilderData.mVertexVariables.mVertexInputs.push_back(*it);
    }

    if(mMaterial->hasTexture(TextureMap::BASE_COLOR))
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mTextureCoord);
    }
    
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mColor))
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mColor);
    }
    
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mNormal))
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mNormal);
    }
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mFragPosition);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mInstanceID);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mObjectID);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mMaterialInstanceID);
    
    if(mMaterial->hasTexture(TextureMap::BASE_COLOR))
    {
        shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mTextureCoord);
    }
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mColor);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mNormal))
    {
        shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mNormal);
    }
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mFragPosition);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mInstanceID);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mObjectID);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mMaterialInstanceID);
    shaderBuilderData.mFragmentVariables.mFragmentOutputs.push_back(GPUBuiltIn::FragmentOutput::mColor);

    if(mMaterial->getMaterialData().mReceiveLight)
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(LightBuiltIn::mLightsBufferData);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mDirectionalLightStructDefinition);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mPointLightStructDefinition);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(LightBuiltIn::mSpotLightStructDefinition);
    }
}

void MaterialRuntimeDefault::registerVertexShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
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

    if(mMaterial->getMaterialData().mIsSkinned)
    {
        registerFunctionCalculateBoneTransform(shaderBuilder);
    }
}

void MaterialRuntimeDefault::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
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

void MaterialRuntimeDefault::createVertexShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    registerVertexShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);

    vertexShaderCalculateProjectionViewMatrix(shaderBuilder);

    if(mMaterial->getMaterialData().mIsSkinned)
    {
        vertexShaderCalculateBoneMatrix(shaderBuilder);
    }

    vertexShaderCalculatePositionOutput(shaderBuilder);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mNormal))
    {
        vertexShaderCalculateNormalOutput(shaderBuilder);
    }

    if(mMaterial->hasTexture(TextureMap::BASE_COLOR))
    {
        vertexShaderCalculateTextureCoordinateOutput(shaderBuilder);
    }

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUBuiltIn::VertexInput::mColor))
    {
        vertexShaderCalculateVertexColorOutput(shaderBuilder);
    }

    vertexShaderCalculateInstanceIdOutput(shaderBuilder);
}

void MaterialRuntimeDefault::createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    
    fragmentShaderCode(shaderBuilder);
}

void MaterialRuntimeDefault::registerFunctionCalculateBoneTransform(ShaderBuilder& shaderBuilder) const
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