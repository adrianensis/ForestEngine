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

void Material::internalInit(const MaterialData& materialData, u32 id)
{
    mMaterialData = materialData;
	mID = id;

    loadTextures();

    PoolHandler<Material> handler = GET_SYSTEM(MaterialManager).getMaterialHandler(mID);
    mShader->init(handler);
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
        if (hasTexture((TextureMap)i))
        {
            mTextures[i].get().enable(textureUnit);
            textureUnit++;
        }
    }

    GET_SYSTEM(GPUInterface).setFaceMode(mMaterialData.mCullFaceType);
}

void Material::disable() const
{
	PROFILER_CPU()
    FOR_RANGE(i, 0, mMaterialData.mTextureBindings.size())
    {
        if (hasTexture((TextureMap)i))
        {
            mTextures[i].get().disable();
        }
    }
}

bool Material::hasTexture(TextureMap textureMap) const
{
    return mTextures[(u32)textureMap].isValid();
}


void Material::bindToShader(Ptr<GPUProgram> gpuProgram) const
{
    gpuProgram->enable();

    PoolHandler<Material> handler = GET_SYSTEM(MaterialManager).getMaterialHandler(mID);
    gpuProgram->bindSharedBuffer(GET_SYSTEM(RenderSharedContext).getMaterialPropertiesGPUSharedBuffer(handler));

    u32 textureUnit = 0;
    FOR_ARRAY(i, getMaterialData().mTextureBindings)
    {
        if(hasTexture((TextureMap)i))
        {
            gpuProgram->bindUniformValue<i32>(GPUBuiltIn::Uniforms::getSampler(EnumsManager::toString<TextureMap>((TextureMap)i)).mName, textureUnit);
            textureUnit++;
        }
    }
    
    gpuProgram->disable();
}

void Material::loadTextures()
{
    if(mMaterialData.mIsFont)
    {
        CHECK_MSG(!mMaterialData.mFontData.mPath.empty(), "mMaterialData.mFontData.mPath cannot be empty!");
        TextureData textureData;
        textureData.mPath = mMaterialData.mFontData.mPath;
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
                mTextures[i] = GET_SYSTEM(MaterialManager).loadTexture(textureData);
            }
        }
    }
}

