#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Shader/ShaderBuilder/ShaderBuilderFunctionsLibrary.hpp"

void Material::init(const MaterialData& materialData, u32 id)
{
    mMaterialData = materialData;
	mID = id;

    mInstancedProperties.push_back(MaterialInstancedPropertiesGPUData::mColor);
    mInstancedProperties.push_back(MaterialInstancedPropertiesGPUData::mTextureRegionLeftTop);
    mInstancedProperties.push_back(MaterialInstancedPropertiesGPUData::mTextureRegionSize);
    mInstancedProperties.push_back(MaterialInstancedPropertiesGPUData::mDepth);

    std::vector<GPUStructDefinition::GPUStructVariable> gpuStructVariableaArray;
    FOR_LIST(it, mInstancedProperties)
    {
        const GPUVariableData& gpuVariableData = *it;
        gpuStructVariableaArray.push_back({gpuVariableData.mGPUDataType, gpuVariableData.mName});
    }

    mInstancedPropertiesStructDefinition = 
    {
        "instancedPropertiesStruct",
        {
            gpuStructVariableaArray
        }
    };

    GPUDataType instancedPropertiesStructDataType = {mInstancedPropertiesStructDefinition.mName, mInstancedPropertiesStructDefinition.getTypeSizeInBytes(), GPUPrimitiveDataType::STRUCT};
    
    mInstancedPropertiesSharedBufferData = {
        GPUBufferType::STORAGE,
        {
            {{GPUStorage::UNIFORM, instancedPropertiesStructDataType, "instancedPropertiesArray"}, "", " "}
        },
        "InstancedProperties",
        "instancedProperties"
    };

    loadTextures();
}

void Material::enable() const
{
	PROFILER_CPU()
	if (mTextures[(u32)TextureType::BASE_COLOR])
	{
        mTextures[(u32)TextureType::BASE_COLOR]->enable();
	}
}

void Material::disable() const
{
	PROFILER_CPU()
	if (mTextures[(u32)TextureType::BASE_COLOR])
	{
        mTextures[(u32)TextureType::BASE_COLOR]->disable();
	}
}

bool Material::hasTexture() const
{
    return mTextures[(u32)TextureType::BASE_COLOR].isValid();
}

void Material::loadTextures()
{
    FOR_RANGE(i, 0, mMaterialData.mTexturePaths.size())
    {
        if(!mMaterialData.mTexturePaths[i].empty())
        {
            TextureData textureData;
            textureData.mPath = mMaterialData.mTexturePaths[i];
            textureData.mCreateMipMap = mMaterialData.mCreateMipMap;
                
            mTextures[i] = GET_SYSTEM(MaterialManager).loadTexture(textureData);
        }
    }
}

MaterialInstance Material::createMaterialInstance() const
{
    MaterialInstance instance;
    instance.mMaterial = getPtrToThis();
    instance.mMaterialInstancedProperties = mMaterialData.mMaterialInstancedProperties;
    return instance;
}

void MaterialFont::loadTextures()
{
    CHECK_MSG(!mMaterialData.mFontData.mPath.empty(), "mMaterialData.mFontData.mPath cannot be empty!");
    TextureData textureData;
    textureData.mPath = mMaterialData.mFontData.mPath;
    textureData.mCreateMipMap = mMaterialData.mCreateMipMap;
    textureData.mFontData = mMaterialData.mFontData;
    mTextures[(u32)TextureType::BASE_COLOR] = Ptr<const Texture>::cast(GET_SYSTEM(MaterialManager).loadTextureFont(textureData));
}

using namespace ShaderBuilderNodes;
using namespace ShaderBuilderNodes::Expressions;

void Material::vertexShaderCalculateBoneMatrix(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    
    Variable boneMatrix;
    mainFunc.body().
    variable(boneMatrix, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "boneMatrix", call(GPUBuiltIn::Functions::mCalculateBoneTransform.mName, {}));

    shaderBuilder.setVariableInCache(boneMatrix);
}

void Material::vertexShaderCalculatePositionOutput(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& position = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mPosition.mName);

    Variable finalPositon;
    mainFunc.body().
    variable(finalPositon, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "finalPositon", call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {position, {"1.0f"}}));

    Variable instancedProperties(getInstancedPropertiesSharedBufferData().getScopedGPUVariableData(0));
    Variable depth = {getInstancedPropertiesStructDefinition().mPrimitiveVariables[3]};
    auto& instanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mInstanceID.mName);
    if(mMaterialData.mUseDepth)
    {
        mainFunc.body().
        set(finalPositon.dot("z"), instancedProperties.at(instanceId).dot(depth));
    }

    if(mMaterialData.mIsSkinned)
    {
        Variable boneMatrix = shaderBuilder.getVariableFromCache("boneMatrix");
        mainFunc.body().
        set(finalPositon, boneMatrix.mul(finalPositon));
    }

    if(mMaterialData.mUseModelMatrix)
    {
        Variable modelMatrices;
        auto& modelMatricesBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices.mInstanceName);
        modelMatrices = Variable(modelMatricesBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
        mainFunc.body().set(finalPositon, modelMatrices.at(instanceId).mul(finalPositon));
    }
        
    Variable PVMatrix = shaderBuilder.getVariableFromCache("PV_Matrix");
    mainFunc.body().
    set(GPUBuiltIn::VertexOutput::mPosition, PVMatrix.mul(finalPositon));

    auto& fragPosition = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mFragPosition.mName);
    mainFunc.body().set(fragPosition, call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {finalPositon}));
}

void Material::vertexShaderCalculateNormalOutput(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& normal = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mNormal.mName);

    Variable finalNormal;
    mainFunc.body().
    variable(finalNormal, GPUBuiltIn::PrimitiveTypes::mVector3.mName, "finalNormal", call(GPUBuiltIn::PrimitiveTypes::mVector3.mName, {normal}));

    if(mMaterialData.mIsSkinned)
    {
        Variable boneMatrix = shaderBuilder.getVariableFromCache("boneMatrix");
        Variable transformedNormal;
        mainFunc.body().
        variable(transformedNormal, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "transformedNormal", boneMatrix.mul(call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {finalNormal, {"1.0f"}}))).
        set(finalNormal, transformedNormal.dot("xyz"));
    }

    // if(mMaterialData.mUseModelMatrix)
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

void Material::vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);

    auto& color = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mColor.mName);
    auto& textureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mTextureCoord.mName);
    auto& outTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord.mName);
    mainFunc.body().
    set(outTextureCoord, textureCoord);

    Variable instancedProperties(getInstancedPropertiesSharedBufferData().getScopedGPUVariableData(0));
    Variable textureRegionLeftTop = {getInstancedPropertiesStructDefinition().mPrimitiveVariables[1]};
    Variable textureRegionSize = {getInstancedPropertiesStructDefinition().mPrimitiveVariables[2]};
    auto& instanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mInstanceID.mName);
    mainFunc.body().
    set(outTextureCoord, call(GPUBuiltIn::PrimitiveTypes::mVector2.mName,
    {
        outTextureCoord.dot("x").mul(instancedProperties.at(instanceId).dot(textureRegionSize).dot("x")).add(instancedProperties.at(instanceId).dot(textureRegionLeftTop).dot("x")),
        outTextureCoord.dot("y").mul(instancedProperties.at(instanceId).dot(textureRegionSize).dot("y")).add(instancedProperties.at(instanceId).dot(textureRegionLeftTop).dot("y"))
    }));
}

void Material::vertexShaderCalculateVertexColorOutput(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& color = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mColor.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    mainFunc.body().
    set(outColor, color);
}

void Material::vertexShaderCalculateInstanceIdOutput(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& instanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexInput::mInstanceID.mName);
    auto& outInstanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mInstanceID.mName);

    if(!instanceId.isEmpty())
    {
        mainFunc.body().
        set(outInstanceId, instanceId);
    }
}

void Material::vertexShaderCalculateProjectionViewMatrix(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& globalDataBuffer = shaderBuilder.get().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData.mInstanceName);    
    Variable projectionMatrix(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(0));
    Variable viewMatrix(globalDataBuffer.mGPUSharedBufferData.getScopedGPUVariableData(1));
    Variable PVMatrix;
    mainFunc.body().variable(PVMatrix, GPUBuiltIn::PrimitiveTypes::mMatrix4.mName, "PV_Matrix", projectionMatrix.mul(viewMatrix));

    shaderBuilder.setVariableInCache(PVMatrix);
}

void Material::fragmentShaderBaseColor(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& inColor = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mColor.mName);
    auto& instanceId = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mInstanceID.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);
    
    Variable baseColor;
    mainFunc.body().
    variable(baseColor, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "baseColor", call(GPUBuiltIn::PrimitiveTypes::mVector4.mName, {{"0.0"}, {"0.0"}, {"0.0"}, {"1.0"}}));

    if(mMaterialData.mUseVertexColor)
    {
        mainFunc.body().
        set(baseColor, inColor);
    }
    else
    {
        Variable instancedProperties(getInstancedPropertiesSharedBufferData().getScopedGPUVariableData(0));
        Variable instanceColor = {getInstancedPropertiesStructDefinition().mPrimitiveVariables[0]};
        mainFunc.body().
        set(baseColor, instancedProperties.at(instanceId).dot(instanceColor));
    }

    mainFunc.body().
    set(outColor, baseColor);

    shaderBuilder.setVariableInCache(baseColor);
}

void Material::fragmentShaderTexture(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& inTextureCoord = shaderBuilder.get().getAttribute(GPUBuiltIn::VertexOutput::mTextureCoord.mName);
    auto& sampler = shaderBuilder.get().getAttribute(GPUBuiltIn::Uniforms::mSampler.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);
    
    mainFunc.body().
    set(outColor, call("texture2D", {sampler, inTextureCoord}));

    if(mMaterialData.mUseColorAsTint)
    {
        Variable baseColor = shaderBuilder.getVariableFromCache("baseColor");

        mainFunc.body().
        set(outColor.dot("r"), outColor.dot("r").add(baseColor.dot("r"))).
        set(outColor.dot("g"), outColor.dot("g").add(baseColor.dot("g"))).
        set(outColor.dot("b"), outColor.dot("b").add(baseColor.dot("b"))).
        set(outColor.dot("a"), outColor.dot("a").add(baseColor.dot("a")));
    }
}

void Material::fragmentShaderShadingModel(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);

    Variable phong;
    mainFunc.body().
    variable(phong, GPUBuiltIn::PrimitiveTypes::mVector4.mName, "phong", call(GPUBuiltIn::Functions::mCalculatePhong.mName, {})).
    set(outColor, outColor.mul(phong));
}

void Material::fragmentShaderAlphaDiscard(ShaderBuilder& shaderBuilder) const
{
    auto& mainFunc = shaderBuilder.get().getFunctionDefinition(GPUBuiltIn::Functions::mMain.mName);
    auto& outColor = shaderBuilder.get().getAttribute(GPUBuiltIn::FragmentOutput::mColor.mName);
    
    mainFunc.body().
    ifBlock(outColor.dot("r").add(outColor.dot("g").add(outColor.dot("b"))).eq({"0"})).
        line("discard").
    end();
}

ShaderBuilderData Material::generateShaderBuilderData(const GPUBuffersLayout& gpuBuffersLayout) const
{
    ShaderBuilderData shaderBuilderData;
    
    shaderBuilderData.mCommonVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::mSampler);

    shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(mInstancedPropertiesStructDefinition);

    FOR_LIST(it, gpuBuffersLayout.getSharedBuffers())
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(it->getGPUSharedBufferData());
    }

    shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mGlobalData);

    if(mMaterialData.mReceiveLight)
    {
        shaderBuilderData.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mLightsData);
        shaderBuilderData.mCommonVariables.mStructDefinitions.push_back(GPUBuiltIn::StructDefinitions::mLight);
    }

    if(mMaterialData.mIsSkinned)
    {
        shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::MAX_BONES);
        shaderBuilderData.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::MAX_BONE_INFLUENCE);
    }

    FOR_LIST(it, gpuBuffersLayout.getVertexBuffers())
    {
        shaderBuilderData.mVertexVariables.mVertexInputs.push_back(*it);
    }

    if(hasTexture())
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mTextureCoord);
    }
    
    if(mMaterialData.mUseVertexColor)
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mColor);
    }
    
    if(mMaterialData.mUseNormals)
    {
        shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mNormal);
    }
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mFragPosition);
    shaderBuilderData.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mInstanceID);
    
    if(hasTexture())
    {
        shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mTextureCoord);
    }
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mColor);

    if(mMaterialData.mUseNormals)
    {
        shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mNormal);
    }
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mFragPosition);
    shaderBuilderData.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mInstanceID);
    shaderBuilderData.mFragmentVariables.mFragmentOutputs.push_back(GPUBuiltIn::FragmentOutput::mColor);

    return shaderBuilderData;
}

void Material::registerVertexShaderData(ShaderBuilder& shaderBuilder, const GPUBuffersLayout& gpuBuffersLayout) const
{
    ShaderBuilderData shaderBuilderData = generateShaderBuilderData(gpuBuffersLayout);
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mStructDefinitions) { shaderBuilder.get().structType(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mVertexVariables.mVertexInputs) { shaderBuilder.get().attribute({it->mData.mGPUVariableData, it->getAttributeLocation()}); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mSharedBuffers) { shaderBuilder.get().sharedBuffer(*it); }
    FOR_LIST(it, shaderBuilderData.mVertexVariables.mVertexOutputs) { shaderBuilder.get().attribute(*it); }

    ShaderBuilderFunctionsLibrary shaderBuilderFunctionsLibrary;
    shaderBuilderFunctionsLibrary.init(shaderBuilder.get(), getPtrToThis());
    if(mMaterialData.mIsSkinned)
    {
        shaderBuilder.get().function(shaderBuilderFunctionsLibrary.mFunctions.at(GPUBuiltIn::Functions::mCalculateBoneTransform.mName));
    }
}

void Material::registerFragmentShaderData(ShaderBuilder& shaderBuilder, const GPUBuffersLayout& gpuBuffersLayout) const
{
    ShaderBuilderData shaderBuilderData = generateShaderBuilderData(gpuBuffersLayout);
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mStructDefinitions) { shaderBuilder.get().structType(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mConsts) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mUniforms) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mCommonVariables.mSharedBuffers) { shaderBuilder.get().sharedBuffer(*it); }
    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mFragmentInputs) { shaderBuilder.get().attribute(*it); }
    FOR_LIST(it, shaderBuilderData.mFragmentVariables.mFragmentOutputs) { shaderBuilder.get().attribute(*it); }

    ShaderBuilderFunctionsLibrary shaderBuilderFunctionsLibrary;
    shaderBuilderFunctionsLibrary.init(shaderBuilder.get(), getPtrToThis());
    if(mMaterialData.mReceiveLight)
    {
        shaderBuilder.get().function(shaderBuilderFunctionsLibrary.mFunctions.at(GPUBuiltIn::Functions::mCalculatePhong.mName));
    }
}

void Material::createVertexShader(ShaderBuilder& shaderBuilder, const GPUBuffersLayout& gpuBuffersLayout) const
{
    registerVertexShaderData(shaderBuilder, gpuBuffersLayout);

    auto& mainFunc = shaderBuilder.get().function(GPUBuiltIn::Functions::mMain);

    vertexShaderCalculateProjectionViewMatrix(shaderBuilder);

    if(mMaterialData.mIsSkinned)
    {
        vertexShaderCalculateBoneMatrix(shaderBuilder);
    }

    vertexShaderCalculatePositionOutput(shaderBuilder);

    if(mMaterialData.mUseNormals)
    {
        vertexShaderCalculateNormalOutput(shaderBuilder);
    }

    if(hasTexture())
    {
        vertexShaderCalculateTextureCoordinateOutput(shaderBuilder);
    }

    if(mMaterialData.mUseVertexColor)
    {
        vertexShaderCalculateVertexColorOutput(shaderBuilder);
    }

    vertexShaderCalculateInstanceIdOutput(shaderBuilder);
}

void Material::createFragmentShader(ShaderBuilder& shaderBuilder, const GPUBuffersLayout& gpuBuffersLayout) const
{
    registerFragmentShaderData(shaderBuilder, gpuBuffersLayout);

    auto& mainFunc = shaderBuilder.get().function(GPUBuiltIn::Functions::mMain);

    fragmentShaderBaseColor(shaderBuilder);

    if(hasTexture())
    {
        fragmentShaderTexture(shaderBuilder);
    }

    if(mMaterialData.mReceiveLight && mMaterialData.mUseNormals)
    {
        fragmentShaderShadingModel(shaderBuilder);
    }
    
    if(mMaterialData.mAlphaEnabled)
    {
        fragmentShaderAlphaDiscard(shaderBuilder);
    }
}

IMPLEMENT_SERIALIZATION(Material)
{
}

IMPLEMENT_DESERIALIZATION(Material)
{
}
