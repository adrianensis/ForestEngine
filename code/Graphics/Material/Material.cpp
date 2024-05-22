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
    FOR_MAP(it, mTextures)
    {
        it->second.reset();
    }
}

void Material::enable() const
{
	PROFILER_CPU()
    u32 textureUnit = 0;
    FOR_MAP(it, mTextures)
    {
        mTextures.at(it->first).get().enable(textureUnit);
        textureUnit++;
    }

    GET_SYSTEM(GPUInterface).setFaceMode(mMaterialData.mCullFaceType);
}

void Material::disable() const
{
	PROFILER_CPU()
    FOR_MAP(it, mTextures)
    {
        mTextures.at(it->first).get().disable();
    }
}

bool Material::hasTexture(ConstString bindingName) const
{
    
    return mTextures.contains(bindingName) && mTextures.at(bindingName).isValid();
}

void Material::bindToShader(Ptr<GPUProgram> gpuProgram) const
{
    gpuProgram->enable();

    PoolHandler<Material> handler = GET_SYSTEM(MaterialManager).getMaterialHandler(mID);
    gpuProgram->bindSharedBuffer(GET_SYSTEM(RenderSharedContext).getMaterialPropertiesGPUSharedBuffer(handler));

    u32 textureUnit = 0;
    FOR_MAP(it, mTextures)
    {
        gpuProgram->bindUniformValue<i32>(GPUBuiltIn::Uniforms::getSampler(it->first).mName, textureUnit);
        textureUnit++;
    }
    
    gpuProgram->disable();
}

void Material::loadTextures()
{
    FOR_MAP(it, mMaterialData.mTextureBindings)
    {
        CHECK_MSG(!it->second.mPath.empty(), "texture mPath cannot be empty!");
        TextureData textureData;
        textureData.mPath = it->second.mPath;
        textureData.mStage = it->second.mStage;

        if(mMaterialData.mIsFont)
        {
            CHECK_MSG(!mMaterialData.mFontData.mPath.empty(), "mMaterialData.mFontData.mPath cannot be empty!");
            textureData.mIsFont = true;
            textureData.mFontData = mMaterialData.mFontData;
        }

        mTextures.insert_or_assign(it->first, GET_SYSTEM(MaterialManager).loadTexture(textureData));
    }
}

