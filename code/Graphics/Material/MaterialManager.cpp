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
	
    mNoTextureMaterial = createMaterial(materialData);
    mMaterials.get(mNoTextureMaterial).init(materialData, mNoTextureMaterial.getIndex());
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

PoolHandler<Material> MaterialManager::createMaterial(const MaterialData& materialData)
{
    PoolHandler<Material> handler = mMaterials.allocate();
    Material& material = mMaterials.get(handler);
    material.init(materialData, handler.getIndex());

    return handler;
}

const Material& MaterialManager::getMaterial(const PoolHandler<Material>& handler) const
{
    CHECK_MSG(handler.isValid(), "Invalid handler!");
    return mMaterials.get(handler);
}