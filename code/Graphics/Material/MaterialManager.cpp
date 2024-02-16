#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"

void MaterialManager::init()
{
	LOG_TRACE()

	// reserve index 0 for no textured material
    MaterialData materialData;
    materialData.mAlphaEnabled = true;
    materialData.mUseColorAsTint = true;
	
    mNoTextureMaterial = createMaterial(materialData);mMaterials[mMaterialIDCounter].init(materialData, mMaterialIDCounter);
	mMaterialIDCounter++;
}

Handler MaterialManager::loadTexture(const TextureData& textureData)
{
    Handler handler;
	if (!mTexturesByPath.contains(textureData.mPath))
	{
        CHECK_MSG(mTextureIDCounter < smMaxTextures, "Max textures reached!");
        handler = Handler(mTextureIDCounter);
        Texture& texure = mTextures.at(handler.getIndex());
        mTexturesByPath.insert_or_assign(textureData.mPath, handler);
        texure.init(textureData, mTextureIDCounter);
        mTextureIDCounter++;
	}

	return handler;
}

Handler MaterialManager::createMaterial(const MaterialData& materialData)
{
    CHECK_MSG(mMaterialIDCounter < smMaxMaterials, "Max materials reached!");
    Handler handler(mMaterialIDCounter);
    Material& material = mMaterials.at(handler.getIndex());
    material.init(materialData, handler.getIndex());
    mMaterialIDCounter++;

    return handler;
}

const Material& MaterialManager::getMaterial(const Handler& handler) const
{
    CHECK_MSG(handler.isValid(), "Invalid handler!");
    return mMaterials.at(handler.getIndex());
}

const Texture& MaterialManager::getTexture(const Handler& handler) const
{
    CHECK_MSG(handler.isValid(), "Invalid handler!");
    return mTextures.at(handler.getIndex());
}