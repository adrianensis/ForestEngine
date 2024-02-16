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

PoolHandler<Texture> MaterialManager::loadTexture(const TextureData& textureData)
{
	if (!mTexturesByPath.contains(textureData.mPath))
	{
        PoolHandler<Texture> handler = mTextures.allocate();
        mTexturesByPath.insert_or_assign(textureData.mPath, handler);
        Texture& texure = mTextures.get(handler);
        texure.init(textureData, handler.getIndex());
	}

	return mTexturesByPath.at(textureData.mPath);
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