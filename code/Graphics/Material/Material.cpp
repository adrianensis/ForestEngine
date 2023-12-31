#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Model.hpp"

void Material::init(const MaterialData& materialData, u32 id)
{
    mMaterialData = materialData;
	mID = id;

    loadTextures();
    

    mMaterialShaderVariables.mCommonVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::mTime);
    mMaterialShaderVariables.mCommonVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::mWindowSize);
    mMaterialShaderVariables.mCommonVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::mBaseColor);
    mMaterialShaderVariables.mCommonVariables.mUniforms.push_back(GPUBuiltIn::Uniforms::mSampler);

    mMaterialShaderVariables.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mGlobalMatrices);

    if(materialData.mUseModelMatrix)
    {
        mMaterialShaderVariables.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mModelMatrices);
    }

    if(materialData.mReceiveLight)
    {
        mMaterialShaderVariables.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mLights);
        mMaterialShaderVariables.mCommonVariables.mStructDefinitions.push_back(GPUBuiltIn::StructDefinitions::mLight);
    }

    if(materialData.mIsSkinned)
    {
        mMaterialShaderVariables.mCommonVariables.mSharedBuffers.push_back(GPUBuiltIn::SharedBuffers::mBonesMatrices);
        mMaterialShaderVariables.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::MAX_BONES);
        mMaterialShaderVariables.mCommonVariables.mConsts.push_back(GPUBuiltIn::Consts::MAX_BONE_INFLUENCE);
    }

    if(hasTexture())
    {
        mMaterialShaderVariables.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mTextureCoord);
    }
    
    if(mMaterialData.mUseVertexColor)
    {
        mMaterialShaderVariables.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mColor);
    }
    
    if(materialData.mUseModelMatrix && materialData.mUseNormals)
    {
        mMaterialShaderVariables.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mNormal);
    }
    mMaterialShaderVariables.mVertexVariables.mVertexOutputs.push_back(GPUBuiltIn::VertexOutput::mFragPosition);
    
    if(hasTexture())
    {
        mMaterialShaderVariables.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mTextureCoord);
    }

    mMaterialShaderVariables.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mColor);

    if(materialData.mUseModelMatrix && materialData.mUseNormals)
    {
        mMaterialShaderVariables.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mNormal);
    }
    mMaterialShaderVariables.mFragmentVariables.mFragmentInputs.push_back(GPUBuiltIn::FragmentInput::mFragPosition);

    mMaterialShaderVariables.mFragmentVariables.mFragmentOutputs.push_back(GPUBuiltIn::FragmentOutput::mColor);
}

void Material::bind(Ptr<GPUShader> shader, bool isWorldSpace, bool isInstanced, Ptr<const Mesh> mesh) const
{
	PROFILER_CPU()

	if (mTextures[(u32)TextureType::BASE_COLOR])
	{
        mTextures[(u32)TextureType::BASE_COLOR]->bind();
	}

	shader->bindUniformValue(GPUBuiltIn::Uniforms::mBaseColor.mName, mMaterialData.mBaseColor);
	shader->bindUniformValue(GPUBuiltIn::Uniforms::mTime.mName, GET_SYSTEM(Time).getDeltaTimeSeconds());
	shader->bindUniformValue(GPUBuiltIn::Uniforms::mWindowSize.mName, GET_SYSTEM(Window).getWindowSize());
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

void MaterialFont::loadTextures()
{
    CHECK_MSG(!mMaterialData.mFontData.mPath.empty(), "mMaterialData.mFontData.mPath cannot be empty!");
    TextureData textureData;
    textureData.mPath = mMaterialData.mFontData.mPath;
    textureData.mCreateMipMap = mMaterialData.mCreateMipMap;
    textureData.mFontData = mMaterialData.mFontData;
    mTextures[(u32)TextureType::BASE_COLOR] = Ptr<const Texture>::cast(GET_SYSTEM(MaterialManager).loadTextureFont(textureData));
}

IMPLEMENT_SERIALIZATION(Material)
{
}

IMPLEMENT_DESERIALIZATION(Material)
{
}
