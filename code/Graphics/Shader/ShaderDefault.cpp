#include "Graphics/Shader/ShaderDefault.hpp"
#include "Graphics/Light/Light.hpp"
using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

void ShaderDefault::registerTextures()
{
    mTextures.insert(TextureBindingNames::smBaseColor);
}

void ShaderDefault::setSharedShaderPropertiesBlock()
{
    mSharedShaderPropertiesBlockBuffer.set<PropertiesBlockShaderDefault>();
    mSharedShaderPropertiesBlockClass = ClassManager::getClassMetadata<PropertiesBlockShaderDefault>().mClassDefinition;
}

std::vector<GPUStructDefinition::GPUStructVariable> ShaderDefault::generateShaderPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {GPUShaderDefinitions::PrimitiveTypes::mVector4, "BaseColor"},
    };

    return propertiesBlock;
}

void ShaderDefault::vertexShaderCalculateBoneMatrix(ShaderBuilder& shaderBuilder) const
{
    Variable boneMatrix;
    shaderBuilder.getMain().
    variable(boneMatrix, GPUShaderDefinitions::PrimitiveTypes::mMatrix4, "boneMatrix", call(GPUShaderDefinitions::Functions::mCalculateBoneTransform, {}));

    shaderBuilder.setVariableInCache(boneMatrix);
}

void ShaderDefault::vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const
{
    auto& position = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mPosition);

    Variable finalPositon;
    shaderBuilder.getMain().
    variable(finalPositon, GPUShaderDefinitions::PrimitiveTypes::mVector4, "finalPositon", call(GPUShaderDefinitions::PrimitiveTypes::mVector4, {position, {"1.0f"}}));
    
    // auto& bonesIDs = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mBonesIDs);
    // if(bonesIDs.isValid())
    // {
    //     Variable boneMatrix = shaderBuilder.getVariableFromCache("boneMatrix");
    //     shaderBuilder.getMain().
    //     set(finalPositon, boneMatrix.mul(finalPositon));
    // }

    auto& modelMatricesBuffer = shaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices.mInstanceName);
    if(modelMatricesBuffer.isValid())
    {
        Variable modelMatrices;
        modelMatrices = Variable(modelMatricesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        auto& objectId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mObjectID);
        if(objectId.isValid())
        {
            shaderBuilder.getMain().set(finalPositon, modelMatrices.at(objectId).mul(finalPositon));
        }
    }

    shaderBuilder.setVariableInCache(finalPositon);
    
    vertexShaderCalculatePositionOutputCustom(shaderBuilder);

    auto& globalDataBuffer = shaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData.mInstanceName);    
    Variable projectionViewMatrix(globalDataBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));

    shaderBuilder.getMain().
    set(GPUShaderDefinitions::VertexOutput::mPosition, projectionViewMatrix.mul(finalPositon));

    // auto& fragPosition = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mFragPosition);
    // shaderBuilder.getMain().set(fragPosition, call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {finalPositon}));
}

void ShaderDefault::vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const
{

}

void ShaderDefault::vertexShaderCalculateNormalOutput(ShaderBuilder& shaderBuilder) const
{
    auto& normal = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mNormal);

    Variable finalNormal;
    shaderBuilder.getMain().
    variable(finalNormal, GPUShaderDefinitions::PrimitiveTypes::mVector3, "finalNormal", call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {normal}));

    auto& bonesIDs = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mBonesIDs);
    if(bonesIDs.isValid())
    {
        Variable boneMatrix = shaderBuilder.getVariableFromCache("boneMatrix");
        Variable transformedNormal;
        shaderBuilder.getMain().
        variable(transformedNormal, GPUShaderDefinitions::PrimitiveTypes::mVector4, "transformedNormal", boneMatrix.mul(call(GPUShaderDefinitions::PrimitiveTypes::mVector4, {finalNormal, {"1.0f"}}))).
        set(finalNormal, transformedNormal.dot("xyz"));
    }

    auto& outNormal = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mNormal);
    auto& modelMatricesBuffer = shaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices.mInstanceName);
    if(modelMatricesBuffer.isValid())
    {
        Variable modelMatrices;
        modelMatrices = Variable(modelMatricesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        auto& objectId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mObjectID);
        if(objectId.isValid())
        {
            /*
                - INFO - 
                There are many sources online that tell you that you need the transpose of the inverse of the world matrix in order to
                transform the normal vector. This is correct, however, we usually don't need to go that far. Our world matrices are
                always orthogonal (their vectors are always orthogonal). Since the inverse of an orthogonal matrix is equal to its transpose,
                the transpose of the inverse is actually the transpose of the transpose, so we end up with the original matrix.
                As long as we avoid doing distortions (scaling one axis differently than the rest) we are fine with the approach I presented above. 
            */
            // shaderBuilder.getMain().
            shaderBuilder.getMain().
            set(outNormal, call("mat3", {call("transpose", {call("inverse", {modelMatrices.at(objectId)})})}).mul(finalNormal));
            // set(outNormal, call(GPUShaderDefinitions::PrimitiveTypes::mMatrix3, {modelMatrices.at(objectId)}).mul(finalNormal));
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
    auto& textureCoord = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0));
    if(textureCoord.isValid())
    {
        auto& outTextureCoord = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mTextureCoords.at(0));
        shaderBuilder.getMain().
        set(outTextureCoord, textureCoord);
    }
}

void ShaderDefault::vertexShaderCalculateVertexColorOutput(ShaderBuilder& shaderBuilder) const
{
    auto& color = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mColor);
    auto& outColor = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mColor);
    shaderBuilder.getMain().
    set(outColor, color);
}

void ShaderDefault::vertexShaderCalculateInstanceIdOutput(ShaderBuilder& shaderBuilder) const
{
    auto& instanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mInstanceID);
    auto& objectId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mObjectID);
    auto& shaderInstanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mShaderInstanceID);
    auto& outInstanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mInstanceID);
    auto& outObjectId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mObjectID);
    auto& outShaderInstanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mShaderInstanceID);

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
    if(shaderInstanceId.isValid())
    {
        shaderBuilder.getMain().
        set(outShaderInstanceId, shaderInstanceId);
    }
}

void ShaderDefault::fragmentShaderCode(ShaderBuilder& shaderBuilder) const
{
    auto& inColor = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mColor);
    auto& outColor = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentOutput::mColor);
    
    auto& shaderInstanceId = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mShaderInstanceID);
    Variable propertiesBlock(mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
    Variable instanceBaseColor = {mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};

    Variable baseColor;
    shaderBuilder.getMain().
    variable(baseColor, GPUShaderDefinitions::PrimitiveTypes::mVector4, "baseColor", propertiesBlock.at(shaderInstanceId).dot(instanceBaseColor));

    if(inColor.isValid())
    {
        shaderBuilder.getMain().
        set(baseColor, inColor);
    }

    shaderBuilder.getMain().
    set(outColor, baseColor);

    auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mTextureCoords.at(0));
    if(inTextureCoord.isValid())
    {
        auto& textureHandler = shaderBuilder.get().getAttribute(GPUShaderDefinitions::Uniforms::getTextureHandler(TextureBindingNames::smBaseColor));
        // auto& texturesBuffer = shaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mTextures.mInstanceName);    
        // Variable textures(texturesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        shaderBuilder.getMain().
        // ifBlock(textureHandler.notEq("0"s)).
            set(outColor, call("texture", {/*textures.at(textureHandler)*/textureHandler, inTextureCoord}));
        // end();
    }
}

void ShaderDefault::generateShaderGenerationData(ShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
    // FOR_MAP(it, mTextures)
    // {
    //     shaderGenerationData.mFragmentVariables.mSamplers.push_back(GPUShaderDefinitions::Uniforms::getTextureHandler(*it));
    // }
    
    // FOR_MAP(it, mShader->getShaderData().mTextureBindings)
    // {
    //     CHECK_MSG(!it->second.mPath.get().empty(), "texture mPath cannot be empty!");

    //     HashedString samplerName = it->first;
    //     switch (it->second.mStage)
    //     {
    //         // case GPUPipelineStage::VERTEX:
    //         // {
    //         //     shaderGenerationData.mVertexVariables.mUniforms.push_back(GPUShaderDefinitions::Uniforms::getTextureHandler(samplerName));
    //         // }
    //         // break;
    //         case GPUPipelineStage::FRAGMENT:
    //         {
    //             shaderGenerationData.mFragmentVariables.mUniforms.push_back(GPUShaderDefinitions::Uniforms::getTextureHandler(samplerName));
    //         }
    //         break;

    //         default:
    //             CHECK_MSG(false, "Invalid Stage for texture binding!");
    //     }
    // }

    // FOR_MAP(it, mFramebufferBindings)
    // {
    //     CHECK_MSG(!it->second.mSamplerName.get().empty(), "frambuffer texture samplerName cannot be empty!");

    //     HashedString samplerName = it->second.mSamplerName;
    //     switch (it->second.mStage)
    //     {
    //         // case GPUPipelineStage::VERTEX:
    //         //     shaderGenerationData.mVertexVariables.mUniforms.push_back(GPUShaderDefinitions::Uniforms::getSampler(samplerName));
    //         // break;
    //         case GPUPipelineStage::FRAGMENT:
    //             shaderGenerationData.mFragmentVariables.mUniforms.push_back(GPUShaderDefinitions::Uniforms::getSampler(samplerName));
    //         break;

    //         default:
    //             CHECK_MSG(false, "Invalid Stage for frambuffer texture binding!");
    //     }
    // }

    shaderGenerationData.mCommonVariables.mStructDefinitions.push_back(mPropertiesBlockStructDefinition);

    shaderGenerationData.mCommonVariables.mUniformBuffers.push_back(GPUShaderDefinitions::UniformBuffers::mGlobalData);
    // shaderGenerationData.mCommonVariables.mUniformBuffers.push_back(GPUShaderDefinitions::UniformBuffers::mTextures);
    shaderGenerationData.mCommonVariables.mUniformBuffers.push_back(GPUShaderDefinitions::UniformBuffers::mModelMatrices);
    shaderGenerationData.mCommonVariables.mUniformBuffers.push_back(mPropertiesBlockUniformBufferData);

    shaderGenerationData.mCommonVariables.mConsts.push_back(GPUShaderDefinitions::Consts::mPI);
    shaderGenerationData.mCommonVariables.mConsts.push_back(GPUShaderDefinitions::Consts::mPI180);
    // if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mBonesIDs))
    // {
    //     shaderGenerationData.mCommonVariables.mConsts.push_back(GPUShaderDefinitions::Consts::mMaxBones);
    //     shaderGenerationData.mCommonVariables.mConsts.push_back(GPUShaderDefinitions::Consts::mMaxBoneInfluence);
    //     shaderGenerationData.mCommonVariables.mUniformBuffers.push_back(GPUShaderDefinitions::UniformBuffers::mBonesMatrices);
    // }

    FOR_LIST(it, gpuVertexBuffersContainer.getVertexBuffers())
    {
        shaderGenerationData.mVertexVariables.mVertexInputs.push_back(*it);
    }

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0)))
    {
        shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mTextureCoords.at(0));
    }
    
    // if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mColor))
    // {
    //     shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mColor);
    // }
    
    // if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mNormal))
    // {
    //     shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mNormal);
    // }

    // shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mFragPosition);
    // shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mFragPositionLight);
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mInstanceID))
    {
        shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mInstanceID);
    }
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mObjectID))
    {
        shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mObjectID);
    }
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mShaderInstanceID))
    {
        shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mShaderInstanceID);
    }
    
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0)))
    {
        shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mTextureCoords.at(0));
    }

    // shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mColor);

    // if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mNormal))
    // {
    //     shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mNormal);
    // }
    // shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mFragPosition);
    // shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mFragPositionLight);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mInstanceID))
    {
        shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mInstanceID);
    }
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mObjectID))
    {
        shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mObjectID);
    }
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mShaderInstanceID))
    {
        shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mShaderInstanceID);
    }

    shaderGenerationData.mFragmentVariables.mFragmentOutputs.push_back(GPUShaderDefinitions::FragmentOutput::mColor);
}

void ShaderDefault::registerVertexShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const
{
    ShaderGenerationData shaderGenerationData;
    generateShaderGenerationData(shaderGenerationData, gpuVertexBuffersContainer);
    FOR_LIST(it, shaderGenerationData.mCommonVariables.mStructDefinitions) { shaderBuilder.get().structType(*it); }
    FOR_LIST(it, shaderGenerationData.mCommonVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderGenerationData.mVertexVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderGenerationData.mVertexVariables.mVertexInputs) { shaderBuilder.get().attribute({it->mData.mGPUVariableData, it->getAttributeLocation()}); }

    // FOR_LIST(it, shaderGenerationData.mCommonVariables.mUniforms) { shaderBuilder.get().attribute(Attribute(*it,binding, gpuShaderDescriptorSets->mGPUShaderDescriptorSetsBindings.mSets.at((*it).mName))); }
    // FOR_LIST(it, shaderGenerationData.mVertexVariables.mUniforms) { shaderBuilder.get().attribute(Attribute(*it,binding, gpuShaderDescriptorSets->mGPUShaderDescriptorSetsBindings.mSets.at((*it).mName))); }

    FOR_LIST(it, shaderGenerationData.mCommonVariables.mUniformBuffers) { shaderBuilder.get().uniformBuffer(UniformBuffer(*it, gpuShaderDescriptorSets->mGPUShaderDescriptorSetsBindings.mBindings.at((*it).mBufferName))); }
    u32 vertexOutputIndex = 0;
    FOR_LIST(it, shaderGenerationData.mVertexVariables.mVertexOutputs) { shaderBuilder.get().attribute(Attribute(*it, vertexOutputIndex)); vertexOutputIndex++; }

    // if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mBonesIDs))
    // {
    //     registerFunctionCalculateBoneTransform(shaderBuilder);
    // }
}

void ShaderDefault::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const
{
    ShaderGenerationData shaderGenerationData;
    generateShaderGenerationData(shaderGenerationData, gpuVertexBuffersContainer);
    FOR_LIST(it, shaderGenerationData.mCommonVariables.mStructDefinitions) { shaderBuilder.get().structType(*it); }
    FOR_LIST(it, shaderGenerationData.mCommonVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderGenerationData.mFragmentVariables.mConsts) { shaderBuilder.get().attribute(*it); }

    // FOR_LIST(it, shaderGenerationData.mCommonVariables.mUniforms) { shaderBuilder.get().attribute(Attribute(*it,binding, gpuShaderDescriptorSets->mGPUShaderDescriptorSetsBindings.mSets.at((*it).mName))); binding++; }
    // FOR_LIST(it, shaderGenerationData.mFragmentVariables.mUniforms) { shaderBuilder.get().attribute(Attribute(*it,binding, gpuShaderDescriptorSets->mGPUShaderDescriptorSetsBindings.mSets.at((*it).mName))); binding++; }
    FOR_LIST(it, shaderGenerationData.mCommonVariables.mUniformBuffers) { shaderBuilder.get().uniformBuffer(UniformBuffer(*it, gpuShaderDescriptorSets->mGPUShaderDescriptorSetsBindings.mBindings.at((*it).mBufferName))); }

    FOR_MAP(it, mTextures)
    {
        if(gpuShaderDescriptorSets->mGPUShaderDescriptorSetsBindings.mBindings.contains(*it))
        {
            shaderGenerationData.mFragmentVariables.mSamplers.push_back(GPUShaderDefinitions::Uniforms::getTextureHandler(*it));
            shaderBuilder.get().attribute(Attribute(GPUShaderDefinitions::Uniforms::getTextureHandler(*it),gpuShaderDescriptorSets->mGPUShaderDescriptorSetsBindings.mBindings.at(*it)));
        }
    }

    // FOR_LIST(it, shaderGenerationData.mFragmentVariables.mSamplers) { shaderBuilder.get().attribute(Attribute(*it,gpuShaderDescriptorSets->mGPUShaderDescriptorSetsBindings.mBindings.at((*it).mName))); }
    u32 fragmentInputIndex = 0;
    FOR_LIST(it, shaderGenerationData.mFragmentVariables.mFragmentInputs) { shaderBuilder.get().attribute(Attribute(*it, fragmentInputIndex)); fragmentInputIndex++; }
    u32 fragmentOutputIndex = 0;
    FOR_LIST(it, shaderGenerationData.mFragmentVariables.mFragmentOutputs) { shaderBuilder.get().attribute(Attribute(*it, fragmentOutputIndex)); fragmentOutputIndex++; }
}

void ShaderDefault::createVertexShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const
{
    registerVertexShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuShaderDescriptorSets);

    // shaderBuilder.get().extension("GL_ARB_bindless_texture");

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mBonesIDs))
    {
        vertexShaderCalculateBoneMatrix(shaderBuilder);
    }

    vertexShaderCalculatePositionOutput(shaderBuilder);

    // if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mNormal))
    // {
    //     vertexShaderCalculateNormalOutput(shaderBuilder);
    // }

    vertexShaderCalculateTextureCoordinateOutput(shaderBuilder);

    // if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mColor))
    // {
    //     vertexShaderCalculateVertexColorOutput(shaderBuilder);
    // }

    vertexShaderCalculateInstanceIdOutput(shaderBuilder);
}

void ShaderDefault::createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, WeakPtr<const GPUShaderDescriptorSets> gpuShaderDescriptorSets) const
{
    registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuShaderDescriptorSets);
    
    // shaderBuilder.get().extension("GL_ARB_bindless_texture");

    fragmentShaderCode(shaderBuilder);
}

void ShaderDefault::registerFunctionCalculateBoneTransform(ShaderBuilder& shaderBuilder) const
{
    FunctionDefinition func(GPUShaderDefinitions::Functions::mCalculateBoneTransform);
    
    auto& bonesIDs = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mBonesIDs);
    auto& bonesWeights = shaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mBonesWeights);
    auto& MAX_BONES = shaderBuilder.get().getAttribute(GPUShaderDefinitions::Consts::mMaxBones);
    auto& MAX_BONE_INFLUENCE = shaderBuilder.get().getAttribute(GPUShaderDefinitions::Consts::mMaxBoneInfluence);
    auto& bonesMatricesblock = shaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mBonesMatrices.mInstanceName);    
    Variable bonesTransform(bonesMatricesblock.mGPUUniformBufferData.getScopedGPUVariableData(0));
    Variable currentBoneTransform;
    Variable currentBoneTransformMulWeight;
    Variable finalBoneTransform;
    func.body().
    variable(finalBoneTransform, GPUShaderDefinitions::PrimitiveTypes::mMatrix4, "finalBoneTransform", call(GPUShaderDefinitions::PrimitiveTypes::mMatrix4, {{"0.0f"}})).
    forBlock("i", "<", MAX_BONE_INFLUENCE, "++").
        ifBlock(bonesIDs.at("i"), "==", {"-1"}).
            line("continue").
        end().
        ifBlock(bonesIDs.at("i"), ">=", MAX_BONES).
            line("break").
        end().
        variable(currentBoneTransform, GPUShaderDefinitions::PrimitiveTypes::mMatrix4, "currentBoneTransform", bonesTransform.at(bonesIDs.at("i"))).
        variable(currentBoneTransformMulWeight, GPUShaderDefinitions::PrimitiveTypes::mMatrix4, "currentBoneTransformMulWeight", currentBoneTransform.mul(bonesWeights.at("i"))).
        set(finalBoneTransform, finalBoneTransform.add(currentBoneTransformMulWeight)).
    end();
    
    func.body().
    ret(finalBoneTransform);

    shaderBuilder.get().function(func);
}