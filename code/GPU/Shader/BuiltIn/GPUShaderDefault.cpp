#include "GPU/Shader/BuiltIn/GPUShaderDefault.hpp"
#include "Core/CoreMacros.hpp"
#include "GPU/Core/GPUDefinitions.h"
#include "GPU/Descriptors/GPUDescriptorLayout.hpp"
#include "GPU/Shader/ShaderBuilder/GPUShaderBuilderNodes.hpp"
using namespace GPUShaderBuilderNodes;
using namespace GPUShaderBuilderNodes::Expressions;

void GPUShaderDefault::registerTextures()
{
    mTextures.insert(TextureBindingNames::smBaseColor);
}

void GPUShaderDefault::setSharedGPUShaderPropertiesBlock()
{
    mSharedGPUShaderPropertiesBlockBuffer.set<PropertiesBlockGPUShaderDefault>();
    mSharedGPUShaderPropertiesBlockId = typeid(PropertiesBlockGPUShaderDefault).hash_code();
}

std::vector<GPUStructDefinition::GPUStructVariable> GPUShaderDefault::generateGPUShaderPropertiesBlock()
{
    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock;
    propertiesBlock.push_back({GPUShaderDefinitions::PrimitiveTypes::mVector4, "BaseColor"});
    propertiesBlock.push_back({GPUShaderDefinitions::PrimitiveTypes::mUnsignedInt, "mBaseColorTextureHandle"});

    return propertiesBlock;
}

void GPUShaderDefault::vertexGPUShaderCalculateBoneMatrix(GPUShaderBuilder& GPUShaderBuilder) const
{
    Variable boneMatrix;
    GPUShaderBuilder.getMain().
    variable(boneMatrix, GPUShaderDefinitions::PrimitiveTypes::mMatrix4, "boneMatrix", call(GPUShaderDefinitions::Functions::mCalculateBoneTransform, {}));

    GPUShaderBuilder.setVariableInCache(boneMatrix);
}

void GPUShaderDefault::vertexGPUShaderCalculatePositionOutput(GPUShaderBuilder& GPUShaderBuilder) const
{
    auto& position = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mPosition);

    Variable finalPositon;
    GPUShaderBuilder.getMain().
    variable(finalPositon, GPUShaderDefinitions::PrimitiveTypes::mVector4, "finalPositon", call(GPUShaderDefinitions::PrimitiveTypes::mVector4, {position, {"1.0f"}}));
    
    auto& bonesIDs = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mBonesIDs);
    if(bonesIDs.isValid())
    {
        Variable boneMatrix = GPUShaderBuilder.getVariableFromCache("boneMatrix");
        GPUShaderBuilder.getMain().
        set(finalPositon, boneMatrix.mul(finalPositon));
    }

    auto& modelMatricesBuffer = GPUShaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices.mInstanceName);
    if(modelMatricesBuffer.isValid())
    {
        Variable modelMatrices;
        modelMatrices = Variable(modelMatricesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        auto& objectId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mObjectID);
        if(objectId.isValid())
        {
            GPUShaderBuilder.getMain().set(finalPositon, modelMatrices.at(objectId).mul(finalPositon));
        }
    }

    GPUShaderBuilder.setVariableInCache(finalPositon);
    
    vertexGPUShaderCalculatePositionOutputCustom(GPUShaderBuilder);

    auto& globalDataBuffer = GPUShaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData.mInstanceName);    
    Variable projectionViewMatrix(globalDataBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));

    GPUShaderBuilder.getMain().
    set(GPUShaderDefinitions::VertexOutput::mPosition, projectionViewMatrix.mul(finalPositon));

    auto& fragPosition = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mFragPosition);
    GPUShaderBuilder.getMain().set(fragPosition, call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {finalPositon}));
}

void GPUShaderDefault::vertexGPUShaderCalculatePositionOutputCustom(GPUShaderBuilder& GPUShaderBuilder) const
{

}

void GPUShaderDefault::vertexGPUShaderCalculateNormalOutput(GPUShaderBuilder& GPUShaderBuilder) const
{
    auto& normal = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mNormal);

    Variable finalNormal;
    GPUShaderBuilder.getMain().
    variable(finalNormal, GPUShaderDefinitions::PrimitiveTypes::mVector3, "finalNormal", call(GPUShaderDefinitions::PrimitiveTypes::mVector3, {normal}));

    auto& bonesIDs = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mBonesIDs);
    if(bonesIDs.isValid())
    {
        Variable boneMatrix = GPUShaderBuilder.getVariableFromCache("boneMatrix");
        Variable transformedNormal;
        GPUShaderBuilder.getMain().
        variable(transformedNormal, GPUShaderDefinitions::PrimitiveTypes::mVector4, "transformedNormal", boneMatrix.mul(call(GPUShaderDefinitions::PrimitiveTypes::mVector4, {finalNormal, {"1.0f"}}))).
        set(finalNormal, transformedNormal.dot("xyz"));
    }

    auto& outNormal = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mNormal);
    auto& modelMatricesBuffer = GPUShaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices.mInstanceName);
    if(modelMatricesBuffer.isValid())
    {
        Variable modelMatrices;
        modelMatrices = Variable(modelMatricesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        auto& objectId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mObjectID);
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
            // GPUShaderBuilder.getMain().
            GPUShaderBuilder.getMain().
            set(outNormal, call("mat3", {call("transpose", {call("inverse", {modelMatrices.at(objectId)})})}).mul(finalNormal));
            // set(outNormal, call(GPUShaderDefinitions::PrimitiveTypes::mMatrix3, {modelMatrices.at(objectId)}).mul(finalNormal));
        }
    }
    else
    {
        GPUShaderBuilder.getMain().
        set(outNormal, finalNormal);
    }
}

void GPUShaderDefault::vertexGPUShaderCalculateTextureCoordinateOutput(GPUShaderBuilder& GPUShaderBuilder) const
{   
    auto& textureCoord = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0));
    if(textureCoord.isValid())
    {
        auto& outTextureCoord = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mTextureCoords.at(0));
        GPUShaderBuilder.getMain().
        set(outTextureCoord, textureCoord);
    }
}

void GPUShaderDefault::vertexGPUShaderCalculateVertexColorOutput(GPUShaderBuilder& GPUShaderBuilder) const
{
    auto& color = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mColor);
    auto& outColor = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mColor);
    GPUShaderBuilder.getMain().
    set(outColor, color);
}

void GPUShaderDefault::vertexGPUShaderCalculateInstanceIdOutput(GPUShaderBuilder& GPUShaderBuilder) const
{
    auto& instanceId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mInstanceID);
    auto& objectId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mObjectID);
    auto& shaderPropertiesInstanceId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mGPUShaderPropertiesInstanceID);
    auto& outInstanceId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mInstanceID);
    auto& outObjectId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mObjectID);
    auto& outGPUShaderPropertiesInstanceId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexOutput::mGPUShaderPropertiesInstanceID);

    if(instanceId.isValid())
    {
        GPUShaderBuilder.getMain().
        set(outInstanceId, instanceId);
    }
    if(objectId.isValid())
    {
        GPUShaderBuilder.getMain().
        set(outObjectId, objectId);
    }
    if(shaderPropertiesInstanceId.isValid())
    {
        GPUShaderBuilder.getMain().
        set(outGPUShaderPropertiesInstanceId, shaderPropertiesInstanceId);
    }
}

void GPUShaderDefault::fragmentGPUShaderCode(GPUShaderBuilder& GPUShaderBuilder) const
{
    auto& inColor = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mColor);
    auto& outColor = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentOutput::mColor);
    
    auto& shaderPropertiesInstanceId = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mGPUShaderPropertiesInstanceID);
    Variable propertiesBlock(mPropertiesBlockUniformBufferData.getScopedGPUVariableData(0));
    Variable instanceBaseColor = {mPropertiesBlockStructDefinition.mPrimitiveVariables[0]};

    Variable baseColor;
    GPUShaderBuilder.getMain().
    variable(baseColor, GPUShaderDefinitions::PrimitiveTypes::mVector4, "baseColor", propertiesBlock.at(shaderPropertiesInstanceId).dot(instanceBaseColor));

    if(inColor.isValid())
    {
        GPUShaderBuilder.getMain().
        set(baseColor, inColor);
    }

    GPUShaderBuilder.getMain().
    set(outColor, baseColor);

    auto& inTextureCoord = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::FragmentInput::mTextureCoords.at(0));
    if(inTextureCoord.isValid())
    {
        auto& textureHandle = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::Uniforms::getTextureHandle(TextureBindingNames::smBaseColor));
        // auto& texturesBuffer = GPUShaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mTextures.mInstanceName);    
        // Variable textures(texturesBuffer.mGPUUniformBufferData.getScopedGPUVariableData(0));
        GPUShaderBuilder.getMain().
        // ifBlock(textureHandle.notEq("0"s)).
            set(outColor, call("texture", {/*textures.at(textureHandle)*/textureHandle, inTextureCoord}));
        // end();
    }
}

void GPUShaderDefault::generateGPUShaderGenerationData(GPUShaderGenerationData& shaderGenerationData, const GPUVertexBuffersContainer& gpuVertexBuffersContainer) const
{
    // FOR_MAP(it, mTextures)
    // {
    //     shaderGenerationData.mFragmentVariables.mSamplers.push_back(GPUShaderDefinitions::Uniforms::getTextureHandle(*it));
    // }
    
    // FOR_MAP(it, mShader->getGPUShaderData().mTextureBindings)
    // {
    //     CHECK_MSG(!it->second.mPath.get().empty(), "texture mPath cannot be empty!");

    //     Core::HashedString samplerName = it->first;
    //     switch (it->second.mStage)
    //     {
    //         // case GPUPipelineStage::VERTEX:
    //         // {
    //         //     shaderGenerationData.mVertexVariables.mUniforms.push_back(GPUShaderDefinitions::Uniforms::getTextureHandle(samplerName));
    //         // }
    //         // break;
    //         case GPUPipelineStage::FRAGMENT:
    //         {
    //             shaderGenerationData.mFragmentVariables.mUniforms.push_back(GPUShaderDefinitions::Uniforms::getTextureHandle(samplerName));
    //         }
    //         break;

    //         default:
    //             CHECK_MSG(false, "Invalid Stage for texture binding!");
    //     }
    // }

    // FOR_MAP(it, mFramebufferBindings)
    // {
    //     CHECK_MSG(!it->second.mSamplerName.get().empty(), "frambuffer texture samplerName cannot be empty!");

    //     Core::HashedString samplerName = it->second.mSamplerName;
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
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mBonesIDs))
    {
        shaderGenerationData.mCommonVariables.mConsts.push_back(GPUShaderDefinitions::Consts::mMaxBones);
        shaderGenerationData.mCommonVariables.mConsts.push_back(GPUShaderDefinitions::Consts::mMaxBoneInfluence);
        shaderGenerationData.mCommonVariables.mUniformBuffers.push_back(GPUShaderDefinitions::UniformBuffers::mBonesMatrices);
    }

    for(auto& it: gpuVertexBuffersContainer.getVertexBuffers())
    {
        shaderGenerationData.mVertexVariables.mVertexInputs.push_back(it);
    }

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0)))
    {
        shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mTextureCoords.at(0));
    }
    
    // if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mColor))
    // {
    //     shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mColor);
    // }
    
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mNormal))
    {
        shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mNormal);
    }

    shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mFragPosition);
    shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mFragPositionLight);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mInstanceID))
    {
        shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mInstanceID);
    }
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mObjectID))
    {
        shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mObjectID);
    }
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mGPUShaderPropertiesInstanceID))
    {
        shaderGenerationData.mVertexVariables.mVertexOutputs.push_back(GPUShaderDefinitions::VertexOutput::mGPUShaderPropertiesInstanceID);
    }
    
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mTextureCoords.at(0)))
    {
        shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mTextureCoords.at(0));
    }

    // shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mColor);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mNormal))
    {
        shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mNormal);
    }

    shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mFragPosition);
    shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mFragPositionLight);

    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mInstanceID))
    {
        shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mInstanceID);
    }
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mObjectID))
    {
        shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mObjectID);
    }
    if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mGPUShaderPropertiesInstanceID))
    {
        shaderGenerationData.mFragmentVariables.mFragmentInputs.push_back(GPUShaderDefinitions::FragmentInput::mGPUShaderPropertiesInstanceID);
    }

    shaderGenerationData.mFragmentVariables.mFragmentOutputs.push_back(GPUShaderDefinitions::FragmentOutput::mColor);
}

void GPUShaderDefault::registerVertexGPUShaderData(GPUShaderBuilder& GPUShaderBuilder, const GPUShaderCompilationData& shaderCompilationData) const
{
    GPUShaderGenerationData shaderGenerationData;
    generateGPUShaderGenerationData(shaderGenerationData, shaderCompilationData.mInputVertexBuffersContainer);
    for(auto& it: shaderGenerationData.mCommonVariables.mStructDefinitions)
    {
        GPUShaderBuilder.get().structType(it);
    }
    for(auto& it: shaderGenerationData.mCommonVariables.mConsts)
    {
        GPUShaderBuilder.get().attribute(it);
    }
    for(auto& it: shaderGenerationData.mVertexVariables.mConsts)
    {
        GPUShaderBuilder.get().attribute(it);
    }
    for(auto& it: shaderGenerationData.mVertexVariables.mVertexInputs)
    {
        GPUShaderBuilder.get().attribute({it.mData.mGPUVariableData, it.getAttributeLocation()});
    }

    for(auto& it: shaderGenerationData.mCommonVariables.mUniformBuffers)
    {
        // TODO: refactor into a function
        GPU::u32 bindingPoint = 0;
        bool found = false;
        FOR_ARRAY(b, shaderCompilationData.mGPUDescriptorSetLocal->mGPUDescriptorLayout.mGPUDescriptorLayoutData.mUniformBuffers)
        {
            bindingPoint = b;
            const GPUUniformBuffer& gpuUniformBuffer = shaderCompilationData.mGPUDescriptorSetLocal->mGPUDescriptorLayout.mGPUDescriptorLayoutData.mUniformBuffers[bindingPoint];
            if(gpuUniformBuffer.getGPUUniformBufferData().mBufferName == it.mBufferName)
            {
                found = true;
                break;
            }
        }
        if(found)
        {
            GPUShaderBuilder.get().uniformBuffer(UniformBuffer(it, bindingPoint));
        }
    }
    GPU::u32 vertexOutputIndex = 0;
    for(auto& it: shaderGenerationData.mVertexVariables.mVertexOutputs)
    {
        GPUShaderBuilder.get().attribute(Attribute(it, vertexOutputIndex)); vertexOutputIndex++;
    }

    if(shaderCompilationData.mInputVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mBonesIDs))
    {
        registerFunctionCalculateBoneTransform(GPUShaderBuilder);
    }
}

void GPUShaderDefault::registerFragmentGPUShaderData(GPUShaderBuilder& GPUShaderBuilder, const GPUShaderCompilationData& shaderCompilationData) const
{
    GPUShaderGenerationData shaderGenerationData;
    generateGPUShaderGenerationData(shaderGenerationData, shaderCompilationData.mInputVertexBuffersContainer);
    for(auto& it: shaderGenerationData.mCommonVariables.mStructDefinitions)
    {
        GPUShaderBuilder.get().structType(it);
    }
    for(auto& it: shaderGenerationData.mCommonVariables.mConsts)
    {
        GPUShaderBuilder.get().attribute(it);
    }
    for(auto& it: shaderGenerationData.mFragmentVariables.mConsts)
    {
        GPUShaderBuilder.get().attribute(it);
    }

    for(auto& it: shaderGenerationData.mCommonVariables.mUniformBuffers)
    {
        // TODO: refactor into a function
        GPU::u32 bindingPoint = 0;
        bool found = false;
        FOR_ARRAY(b, shaderCompilationData.mGPUDescriptorSetLocal->mGPUDescriptorLayout.mGPUDescriptorLayoutData.mUniformBuffers)
        {
            bindingPoint = b;
            const GPUUniformBuffer& gpuUniformBuffer = shaderCompilationData.mGPUDescriptorSetLocal->mGPUDescriptorLayout.mGPUDescriptorLayoutData.mUniformBuffers[bindingPoint];
            if(gpuUniformBuffer.getGPUUniformBufferData().mBufferName == it.mBufferName)
            {
                found = true;
                break;
            }
        }
        if(found)
        {
            GPUShaderBuilder.get().uniformBuffer(UniformBuffer(it, bindingPoint));
        }
    }

    GPUShaderBuilder.get().attribute(Attribute(GPUShaderDefinitions::Uniforms::mTextures, 0, GPUDescriptorSetScope::GLOBAL));

    FOR_MAP(it, mTextures)
    {
        const std::string& textureName = (*it);
        // TODO: refactor into a function
        GPU::u32 bindingPoint = 0;
        bool found = false;
        FOR_ARRAY(b, shaderCompilationData.mGPUDescriptorSetLocal->mGPUDescriptorLayout.mGPUDescriptorLayoutData.mTextureBindings)
        {
            bindingPoint = b;
            const GPUTextureBinding& textureBinding = shaderCompilationData.mGPUDescriptorSetLocal->mGPUDescriptorLayout.mGPUDescriptorLayoutData.mTextureBindings[bindingPoint];
            if(textureBinding.mName == textureName)
            {
                found = true;
                break;
            }
        }
        if(found)
        {
            GPU::u32 bindingPointOffset = shaderCompilationData.mGPUDescriptorSetLocal->mGPUDescriptorLayout.mGPUDescriptorLayoutData.mUniformBuffers.size();
            shaderGenerationData.mFragmentVariables.mSamplers.push_back(GPUShaderDefinitions::Uniforms::getTextureHandle(*it));
            GPUShaderBuilder.get().attribute(Attribute(GPUShaderDefinitions::Uniforms::getTextureHandle(*it),bindingPoint + bindingPointOffset));
        }
    }

    GPU::u32 fragmentInputIndex = 0;
    for(auto& it: shaderGenerationData.mFragmentVariables.mFragmentInputs)
    {
        GPUShaderBuilder.get().attribute(Attribute(it, fragmentInputIndex)); fragmentInputIndex++;
    }
    GPU::u32 fragmentOutputIndex = 0;
    for(auto& it: shaderGenerationData.mFragmentVariables.mFragmentOutputs)
    {
        GPUShaderBuilder.get().attribute(Attribute(it, fragmentOutputIndex)); fragmentOutputIndex++;
    }
}

void GPUShaderDefault::createVertexShader(GPUShaderBuilder& GPUShaderBuilder, const GPUShaderCompilationData& shaderCompilationData) const
{
    registerVertexGPUShaderData(GPUShaderBuilder, shaderCompilationData);

    // GPUShaderBuilder.get().extension("GL_ARB_bindless_texture");

    if(shaderCompilationData.mInputVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mBonesIDs))
    {
        vertexGPUShaderCalculateBoneMatrix(GPUShaderBuilder);
    }

    vertexGPUShaderCalculatePositionOutput(GPUShaderBuilder);

    if(shaderCompilationData.mInputVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mNormal))
    {
        vertexGPUShaderCalculateNormalOutput(GPUShaderBuilder);
    }

    vertexGPUShaderCalculateTextureCoordinateOutput(GPUShaderBuilder);

    // if(gpuVertexBuffersContainer.containsVertexBuffer(GPUShaderDefinitions::VertexInput::mColor))
    // {
    //     vertexGPUShaderCalculateVertexColorOutput(GPUShaderBuilder);
    // }

    vertexGPUShaderCalculateInstanceIdOutput(GPUShaderBuilder);
}

void GPUShaderDefault::createFragmentShader(GPUShaderBuilder& GPUShaderBuilder, const GPUShaderCompilationData& shaderCompilationData) const
{
    registerFragmentGPUShaderData(GPUShaderBuilder, shaderCompilationData);
    
    GPUShaderBuilder.get().extension("GL_EXT_nonuniform_qualifier");

    fragmentGPUShaderCode(GPUShaderBuilder);
}

void GPUShaderDefault::registerFunctionCalculateBoneTransform(GPUShaderBuilder& GPUShaderBuilder) const
{
    FunctionDefinition func(GPUShaderDefinitions::Functions::mCalculateBoneTransform);
    
    auto& bonesIDs = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mBonesIDs);
    auto& bonesWeights = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::VertexInput::mBonesWeights);
    auto& MAX_BONES = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::Consts::mMaxBones);
    auto& MAX_BONE_INFLUENCE = GPUShaderBuilder.get().getAttribute(GPUShaderDefinitions::Consts::mMaxBoneInfluence);
    auto& bonesMatricesblock = GPUShaderBuilder.get().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mBonesMatrices.mInstanceName);    
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

    GPUShaderBuilder.get().function(func);
}