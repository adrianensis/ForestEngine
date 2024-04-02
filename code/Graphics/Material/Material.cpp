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

void Material::init(const MaterialData& materialData, u32 id, MaterialRuntime* runtime)
{
    mMaterialData = materialData;
	mID = id;

    loadTextures();

    mMaterialRuntime = runtime;
}

void Material::terminate()
{
    FOR_RANGE(i, 0, mTextures.size())
    {
        mTextures[i].reset();
    }
}

void Material::enable() const
{
	PROFILER_CPU()
    u32 textureUnit = 0;
    FOR_RANGE(i, 0, mMaterialData.mTextureBindings.size())
    {
        if (mTextures[i].isValid())
        {
            mTextures[i].get().enable(textureUnit);
            textureUnit++;
        }
    }
}

void Material::disable() const
{
	PROFILER_CPU()
    FOR_RANGE(i, 0, mMaterialData.mTextureBindings.size())
    {
        if (mTextures[i].isValid())
        {
            mTextures[i].get().disable();
        }
    }
}

bool Material::hasTexture() const
{
    return mTextures[(u32)TextureMap::BASE_COLOR].isValid();
}

void Material::loadTextures()
{
    if(mMaterialData.mIsFont)
    {
        CHECK_MSG(!mMaterialData.mFontData.mPath.empty(), "mMaterialData.mFontData.mPath cannot be empty!");
        TextureData textureData;
        textureData.mPath = mMaterialData.mFontData.mPath;
        textureData.mCreateMipMap = mMaterialData.mCreateMipMap;
        textureData.mStage = GPUPipelineStage::FRAGMENT;
        textureData.mIsFont = true;
        textureData.mFontData = mMaterialData.mFontData;
        mTextures[(u32)TextureMap::BASE_COLOR] = GET_SYSTEM(MaterialManager).loadTexture(textureData);
    }
    else
    {
        FOR_RANGE(i, 0, mMaterialData.mTextureBindings.size())
        {
            if(!mMaterialData.mTextureBindings[i].mPath.empty())
            {
                TextureData textureData;
                textureData.mPath = mMaterialData.mTextureBindings[i].mPath;
                textureData.mStage = mMaterialData.mTextureBindings[i].mStage;
                textureData.mCreateMipMap = mMaterialData.mCreateMipMap;
                mTextures[i] = GET_SYSTEM(MaterialManager).loadTexture(textureData);
            }
        }
    }
}

