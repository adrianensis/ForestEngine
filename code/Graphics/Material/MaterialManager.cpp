#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/RenderSharedContext.hpp"

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

void MaterialManager::terminate()
{
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

void MaterialManager::unloadTexture(PoolHandler<Texture>& texture)
{
    mTextures.free(texture);
}

PoolHandler<Material> MaterialManager::createMaterial(const MaterialData& materialData)
{
    PoolHandler<Material> handler = mMaterials.allocate();
    Material& material = mMaterials.get(handler);
    material.init(materialData, handler.getIndex());

    GET_SYSTEM(RenderSharedContext).initMaterialInstancePropertiesSharedBuffer(handler);

    return handler;
}

MaterialInstance MaterialManager::createMaterialInstance(const PoolHandler<Material>& handler)
{
    Material& material = mMaterials.get(handler);
    MaterialInstance instance;
    instance.mMaterial = handler;
    instance.mMaterialInstancedPropertiesBuffer = material.getMaterialData().mSharedMaterialInstancedPropertiesBuffer;
    return instance;
}

void MaterialManager::removeMaterial(PoolHandler<Material>& material)
{
    mMaterials.free(material);
}

const Material& MaterialManager::getMaterial(const PoolHandler<Material>& handler) const
{
    CHECK_MSG(handler.isValid(), "Invalid handler!");
    return mMaterials.get(handler);
}