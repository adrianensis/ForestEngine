#include "Graphics/Material/MaterialRuntime.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Model.hpp"

void MaterialRuntime::init(PoolHandler<Material> material)
{
    mMaterial = material;
    
    GPUStructDefinition propertiesBlockStructDefinition =
    {
        "propertiesBlockStruct",
        {
            generateMaterialPropertiesBlock()
        }
    };

    GPUDataType propertiesBlockStructDataType =
    {
        propertiesBlockStructDefinition.mName,
        propertiesBlockStructDefinition.getTypeSizeInBytes(),
        GPUPrimitiveDataType::STRUCT
    };
    
    GPUSharedBufferData propertiesBlockSharedBufferData =
    {
        GPUBufferType::STORAGE,
        {
            {{GPUStorage::UNIFORM, propertiesBlockStructDataType, "propertiesBlockArray"}, "", " "}
        },
        "PropertiesBlock",
        "propertiesBlock"
    };

    mPropertiesBlockStructDefinition = propertiesBlockStructDefinition;
    mPropertiesBlockSharedBufferData = propertiesBlockSharedBufferData;
}

std::vector<GPUStructDefinition::GPUStructVariable> MaterialRuntime::generateMaterialPropertiesBlock()
{
    GPUDataType materialLightingModelStructDataType =
    {
        mLightingModelStructDefinition.mName,
        mLightingModelStructDefinition.getTypeSizeInBytes(),
        GPUPrimitiveDataType::STRUCT
    };

    std::vector<GPUStructDefinition::GPUStructVariable> propertiesBlock = 
    {
        {materialLightingModelStructDataType, "materialLighting"},
        {GPUBuiltIn::PrimitiveTypes::mVector2, "textureRegionLeftTop"},
        {GPUBuiltIn::PrimitiveTypes::mVector2, "textureRegionSize"},
        {GPUBuiltIn::PrimitiveTypes::mInt, "depth"},
    };

    return propertiesBlock;
}

using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

void MaterialRuntime::vertexShaderCalculateBoneMatrix(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    
    Variable boneMatrix;
    mainFunc.body().
    variable(boneMatrix, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "boneMatrix", call(GPUBuiltIn::Functions::mCalculateBoneTransform.mName, {}));

    shaderBuilder.setVariableInCache(boneMatrix);
}

void MaterialRuntime::vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& position = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mPosition.mName);

    Variable finalPositon;
    mainFunc.body().
    variable(finalPositon, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "finalPositon", call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {position, {"1.0f"}}));

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable depth = {getPropertiesBlockStructDefinition().mPrimitiveVariables[3]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mMaterialInstanceID.mName);
    if(mMaterial->getMaterialData().mUseDepth)
    {
        mainFunc.body().
        set(finalPositon.dot("z"), propertiesBlock.at(materialInstanceId).dot(depth));
    }

    if(mMaterial->getMaterialData().mIsSkinned)
    {
        Variable boneMatrix = shaderBuilder.getVariableFromCache("boneMatrix");
        mainFunc.body().
        set(finalPositon, boneMatrix.mul(finalPositon));
    }

    if(mMaterial->getMaterialData().mUseModelMatrix)
    {
        Variable modelMatrices;
        auto& modelMatricesBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices.mInstanceName);
        modelMatrices = Variable(modelMatricesBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
        auto& objectId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mObjectID.mName);
        mainFunc.body().set(finalPositon, modelMatrices.at(objectId).mul(finalPositon));
    }
        
    Variable PVMatrix = shaderBuilder.getVariableFromCache("PV_Matrix");
    mainFunc.body().
    set(GPUBuiltIn::VertexOutput::mPosition, PVMatrix.mul(finalPositon));

    auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mFragPosition.mName);
    mainFunc.body().set(fragPosition, call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {finalPositon}));
}

void MaterialRuntime::vertexShaderCalculateNormalOutput(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& normal = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mNormal.mName);

    Variable finalNormal;
    mainFunc.body().
    variable(finalNormal, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "finalNormal", call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {normal}));

    if(mMaterial->getMaterialData().mIsSkinned)
    {
        Variable boneMatrix = shaderBuilder.getVariableFromCache("boneMatrix");
        Variable transformedNormal;
        mainFunc.body().
        variable(transformedNormal, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "transformedNormal", boneMatrix.mul(call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {finalNormal, {"1.0f"}}))).
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
    //     mainFunc.body().
    //     set(outNormal, call("mat3", {call("transpose", {call("inverse", {modelMatrices.at(instanceId)})})}).mul(finalNormal));
    // }
    // else
    // {
        auto& outNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mNormal.mName);
        mainFunc.body().
        set(outNormal, finalNormal);
    // }
}

void MaterialRuntime::vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);

    auto& textureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mTextureCoord.mName);
    auto& outTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord.mName);
    mainFunc.body().
    set(outTextureCoord, textureCoord);

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable textureRegionLeftTop = {getPropertiesBlockStructDefinition().mPrimitiveVariables[1]};
    Variable textureRegionSize = {getPropertiesBlockStructDefinition().mPrimitiveVariables[2]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mMaterialInstanceID.mName);
    mainFunc.body().
    set(outTextureCoord, call(GPUBuiltIn::PrimitiveTypes::mVector2.mName,
    {
        outTextureCoord.dot("x").mul(propertiesBlock.at(materialInstanceId).dot(textureRegionSize).dot("x")).add(propertiesBlock.at(materialInstanceId).dot(textureRegionLeftTop).dot("x")),
        outTextureCoord.dot("y").mul(propertiesBlock.at(materialInstanceId).dot(textureRegionSize).dot("y")).add(propertiesBlock.at(materialInstanceId).dot(textureRegionLeftTop).dot("y"))
    }));
}

void MaterialRuntime::vertexShaderCalculateVertexColorOutput(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& color = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mColor.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    mainFunc.body().
    set(outColor, color);
}

void MaterialRuntime::vertexShaderCalculateInstanceIdOutput(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& instanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mInstanceID.mName);
    auto& objectId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mObjectID.mName);
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mMaterialInstanceID.mName);
    auto& outInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mInstanceID.mName);
    auto& outObjectId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mObjectID.mName);
    auto& outMaterialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID.mName);

    if(!instanceId.isEmpty())
    {
        mainFunc.body().
        set(outInstanceId, instanceId);
    }
    if(!objectId.isEmpty())
    {
        mainFunc.body().
        set(outObjectId, objectId);
    }
    if(!materialInstanceId.isEmpty())
    {
        mainFunc.body().
        set(outMaterialInstanceId, materialInstanceId);
    }
}

void MaterialRuntime::vertexShaderCalculateProjectionViewMatrix(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& globalDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable projectionMatrix(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
    Variable viewMatrix(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(1));
    Variable PVMatrix;
    mainFunc.body().variable(PVMatrix, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "PV_Matrix", projectionMatrix.mul(viewMatrix));

    shaderBuilder.setVariableInCache(PVMatrix);
}

void MaterialRuntime::fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& inColor = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mMaterialInstanceID.mName);
    Variable lightingModel = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);
    
    Variable baseColor;
    mainFunc.body().
    variable(baseColor, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "baseColor", call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {{"0.0"}, {"0.0"}, {"0.0"}, {"1.0"}}));

    if(mMaterial->getMaterialData().mUseVertexColor)
    {
        mainFunc.body().
        set(baseColor, inColor);
    }
    else
    {
        Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
        Variable instanceDiffuse = {mLightingModelStructDefinition.mPrimitiveVariables[1]};
        mainFunc.body().
        set(baseColor, call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(instanceDiffuse), {"1.0"}}));
    }

    mainFunc.body().
    set(outColor, baseColor);

    shaderBuilder.setVariableInCache(baseColor);
}

void MaterialRuntime::fragmentShaderTexture(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord.mName);
    auto& sampler = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::getSampler(std::string(EnumsManager::toString<TextureMap>(TextureMap::BASE_COLOR))).mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);
    
    mainFunc.body().
    set(outColor, call("texture2D", {sampler, inTextureCoord}));

    if(mMaterial->getMaterialData().mAlphaEnabled)
    {
        fragmentShaderAlphaDiscard(shaderBuilder);
    }
    
    if(mMaterial->getMaterialData().mUseColorAsTint)
    {
        Variable baseColor = shaderBuilder.getVariableFromCache("baseColor");
        
        if(mMaterial->getMaterialData().mIsFont)
        {
            mainFunc.body().
            set(outColor.dot("a"), outColor.dot("r"));
            
            mainFunc.body().
            set(outColor.dot("r"), baseColor.dot("r")).
            set(outColor.dot("g"), baseColor.dot("g")).
            set(outColor.dot("b"), baseColor.dot("b"));
        }
        else
        {
            mainFunc.body().
            set(outColor.dot("r"), outColor.dot("r").add(baseColor.dot("r"))).
            set(outColor.dot("g"), outColor.dot("g").add(baseColor.dot("g"))).
            set(outColor.dot("b"), outColor.dot("b").add(baseColor.dot("b"))).
            set(outColor.dot("a"), outColor.dot("a").add(baseColor.dot("a")));
        }
    }
}

void MaterialRuntime::fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);

    Variable phong;
    mainFunc.body().
    variable(phong, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "phong", call(GPUBuiltIn::Functions::mCalculatePhong.mName, {})).
    set(outColor, outColor.mul(phong));
}

void MaterialRuntime::fragmentShaderAlphaDiscard(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);
    
    mainFunc.body().
    ifBlock(outColor.dot("r").add(outColor.dot("g").add(outColor.dot("b"))).eq({"0"})).
        line("discard").
    end();
}

MaterialRuntime::ShaderBuilderData MaterialRuntime::generateShaderBuilderData(const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    ShaderBuilderData shaderBuilderData;
    
    if(mMaterial->getMaterialData().mIsFont)
    {
        std::string samplerName = std::string(EnumsManager::toString<TextureMap>(TextureMap::BASE_COLOR));
        shaderBuilderData.mFragmentVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::getSampler(samplerName));
    }
    else
    {
        FOR_RANGE(i, 0, mMaterial->getMaterialData().mTextureBindings.size())
        {
            if(!mMaterial->getMaterialData().mTextureBindings[i].mPath.empty())
            {
                std::string samplerName = std::string(EnumsManager::toString<TextureMap>(i));

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

    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(mLightingModelStructDefinition);
    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(getPropertiesBlockStructDefinition());

    FOR_LIST(it, gpuSharedBuffersContainer.getSharedBuffers())
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(it->getGPUSharedBufferData());
    }

    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mGlobalData);
    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mModelMatrices);
    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(getPropertiesBlockSharedBufferData());

    if(mMaterial->getMaterialData().mReceiveLight)
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mLightsData);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(GPUBuiltIn::StructDefinitions::mLight);
    }

    if(mMaterial->getMaterialData().mIsSkinned)
    {
        shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mMaxBones);
        shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::mMaxBoneInfluence);
    }

    FOR_LIST(it, gpuVertexBuffersContainer.getVertexBuffers())
    {
        shaderBuilderData.mVertexVariables.mVertexInputs.push_back(*it);
    }

    if(mMaterial->hasTexture())
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mTextureCoord);
    }
    
    if(mMaterial->getMaterialData().mUseVertexColor)
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mColor);
    }
    
    if(mMaterial->getMaterialData().mUseNormals)
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mNormal);
    }
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mFragPosition);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mInstanceID);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mObjectID);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mMaterialInstanceID);
    
    if(mMaterial->hasTexture())
    {
        shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mTextureCoord);
    }
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mColor);

    if(mMaterial->getMaterialData().mUseNormals)
    {
        shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mNormal);
    }
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mFragPosition);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mInstanceID);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mObjectID);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mMaterialInstanceID);
    shaderBuilderData.mFragmentVariables.mFragmentOutputs.push_back(GPUBuiltIn::FragmentOutput::mColor);

    return shaderBuilderData;
}

void MaterialRuntime::registerVertexShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    ShaderBuilderData shaderBuilderData = generateShaderBuilderData(gpuVertexBuffersContainer, gpuSharedBuffersContainer);
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

void MaterialRuntime::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    ShaderBuilderData shaderBuilderData = generateShaderBuilderData(gpuVertexBuffersContainer, gpuSharedBuffersContainer);
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mStructDefinitions) { shaderBuilder.get().structType(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mSharedBuffers) { shaderBuilder.get().sharedBuffer(*it); }
    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mFragmentInputs) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mFragmentOutputs) { shaderBuilder.get().attribute(*it); }

    if(mMaterial->getMaterialData().mReceiveLight)
    {
        registerFunctionCalculatePhong(shaderBuilder);
    }
}

void MaterialRuntime::createVertexShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    registerVertexShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);

    auto& mainFunc = shaderBuilder.get().function(GPUBuiltIn::Functions::mMain);

    vertexShaderCalculateProjectionViewMatrix(shaderBuilder);

    if(mMaterial->getMaterialData().mIsSkinned)
    {
        vertexShaderCalculateBoneMatrix(shaderBuilder);
    }

    vertexShaderCalculatePositionOutput(shaderBuilder);

    if(mMaterial->getMaterialData().mUseNormals)
    {
        vertexShaderCalculateNormalOutput(shaderBuilder);
    }

    if(mMaterial->hasTexture())
    {
        vertexShaderCalculateTextureCoordinateOutput(shaderBuilder);
    }

    if(mMaterial->getMaterialData().mUseVertexColor)
    {
        vertexShaderCalculateVertexColorOutput(shaderBuilder);
    }

    vertexShaderCalculateInstanceIdOutput(shaderBuilder);
}

void MaterialRuntime::createFragmentShader(ShaderBuilder& shaderBuilder, const GPUVertexBuffersContainer& gpuVertexBuffersContainer, const GPUSharedBuffersContainer& gpuSharedBuffersContainer) const
{
    registerFragmentShaderData(shaderBuilder, gpuVertexBuffersContainer, gpuSharedBuffersContainer);

    auto& mainFunc = shaderBuilder.get().function(GPUBuiltIn::Functions::mMain);

    fragmentShaderBaseColor(shaderBuilder);

    if(mMaterial->hasTexture())
    {
        fragmentShaderTexture(shaderBuilder);
    }
    else
    {
        if(mMaterial->getMaterialData().mAlphaEnabled)
        {
            fragmentShaderAlphaDiscard(shaderBuilder);
        }
    }

    if(mMaterial->getMaterialData().mReceiveLight)
    {
        fragmentShaderShadingModel(shaderBuilder);
    }
}

void MaterialRuntime::registerFunctionCalculateBoneTransform(ShaderBuilder& shaderBuilder) const
{
    FunctionDefinition func(GPUBuiltIn::Functions::mCalculateBoneTransform);
    
    auto& bonesIDs = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mBonesIDs.mName);
    auto& bonesWeights = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mBonesWeights.mName);
    auto& MAX_BONES = shaderBuilder.get().getAttribute(GPUBuiltIn::Consts::mMaxBones.mName);
    auto& MAX_BONE_INFLUENCE = shaderBuilder.get().getAttribute(GPUBuiltIn::Consts::mMaxBoneInfluence.mName);
    auto& bonesMatricesblock = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices.mInstanceName);    
    Variable bonesTransform(bonesMatricesblock.mGPUSharedBufferData.getScopedGPUVariableData(0));
    Variable currentBoneTransform;
    Variable currentBoneTransformMulWeight;
    Variable finalBoneTransform;
    func.body().
    variable(finalBoneTransform, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "finalBoneTransform", call(GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, {{"0.0f"}})).
    forBlock("i", "<", MAX_BONE_INFLUENCE, "++").
        ifBlock(bonesIDs.at("i"), "==", {"-1"}).
            line("continue").
        end().
        ifBlock(bonesIDs.at("i"), ">=", MAX_BONES).
            line("break").
        end().
        variable(currentBoneTransform, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "currentBoneTransform", bonesTransform.at(bonesIDs.at("i"))).
        variable(currentBoneTransformMulWeight, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "currentBoneTransformMulWeight", currentBoneTransform.mul(bonesWeights.at("i"))).
        set(finalBoneTransform, finalBoneTransform.add(currentBoneTransformMulWeight)).
    end();
    
    func.body().
    ret(finalBoneTransform);

    shaderBuilder.get().function(func);
}

void MaterialRuntime::registerFunctionCalculatePhong(ShaderBuilder& shaderBuilder) const
{
    FunctionDefinition func(GPUBuiltIn::Functions::mCalculatePhong);

    auto& globalDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable cameraPosition(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(2));
    auto& inNormal = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mNormal.mName);
    auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mFragPosition.mName);
    auto& ligthsDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mLightsData.mInstanceName);    
    Variable lights(ligthsDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
    Variable lightPos = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[0]};
    Variable lightAmbient = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[1]};
    Variable lightDiffuse = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[2]};
    Variable lightSpecular = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[3]};
    Variable lightAmbientIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[4]};
    Variable lightDiffuseIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[5]};
    Variable lightSpecularIntensity = {GPUBuiltIn::StructDefinitions::mLight.mPrimitiveVariables[6]};

    Variable propertiesBlock(getPropertiesBlockSharedBufferData().getScopedGPUVariableData(0));
    Variable lightingModel = {getPropertiesBlockStructDefinition().mPrimitiveVariables[0]};
    Variable materialAmbient = {mLightingModelStructDefinition.mPrimitiveVariables[0]};
    Variable materialDiffuse = {mLightingModelStructDefinition.mPrimitiveVariables[1]};
    Variable materialSpecular = {mLightingModelStructDefinition.mPrimitiveVariables[2]};
    Variable materialShininess = {mLightingModelStructDefinition.mPrimitiveVariables[3]};
    auto& materialInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentInput::mMaterialInstanceID.mName);
    // propertiesBlock.at(materialInstanceId).dot(depth)

    Variable ambient;
    func.body().
    variable(ambient, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "ambient", propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialAmbient).mul(lights.at("0").dot(lightAmbientIntensity).mul(lights.at("0").dot(lightAmbient))));
    Variable norm;
    func.body().
    variable(norm, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "norm", call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {{"0.0"}, {"0.0"}, {"0.0"}}));

    if(mMaterial->getMaterialData().mUseNormals)
    {
        func.body().
        set(norm, call("normalize", {inNormal}));
    }

    Variable lightDir;
    Variable diffuseValue;
    Variable diffuse;
    func.body().
    variable(lightDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "lightDir", call("normalize", {lights.at("0").dot(lightPos).sub(fragPosition)})).
    variable(diffuseValue, GPUBuiltIn::PrimitiveTypes::mFloat.mName, "diffuseValue", call("max", {call("dot", {norm, lightDir}), {"0"}})).
    variable(diffuse, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "diffuse", propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialDiffuse).mul(lights.at("0").dot(lightSpecularIntensity).mul(diffuseValue.mul(lights.at("0").dot(lightDiffuse)))));
    Variable viewDir;
    Variable reflectDir;
    Variable specularValue;
    Variable specular;
    func.body().
    variable(viewDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "viewDir", call("normalize", {cameraPosition.sub(fragPosition)})).
    variable(reflectDir, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "reflectDir", call("reflect", {viewDir.neg(), norm})).
    variable(specularValue, GPUBuiltIn::PrimitiveTypes::mFloat.mName, "specularValue", call("pow", {call("max", {call("dot", {viewDir, reflectDir}), {"0.0"}}), propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialShininess).dot("x")})).
    variable(specular, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "specular", propertiesBlock.at(materialInstanceId).dot(lightingModel).dot(materialSpecular).mul(lights.at("0").dot(lightSpecularIntensity).mul(specularValue).mul(lights.at("0").dot(lightSpecular))));
    Variable phong;
    func.body().
    variable(phong, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "phong", ambient.add(diffuse).add(specular)).
    ret(call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {phong, {"1"}}));

    shaderBuilder.get().function(func);
}
