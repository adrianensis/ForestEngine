#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/RenderState.hpp"

void MaterialManager::init()
{
	LOG_TRACE()
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

void MaterialManager::postMaterialCreated(const PoolHandler<Material>& handler)
{
    GET_SYSTEM(RenderState).initMaterialInstancePropertiesSharedBuffer(handler);
}

MaterialInstance MaterialManager::createMaterialInstance(const PoolHandler<Material>& handler)
{
    Material& material = mMaterials.get(handler);
    MaterialInstance instance;
    instance.mMaterial = handler;
    instance.mMaterialPropertiesBlockBuffer = material.getMaterialData().mSharedMaterialPropertiesBlockBuffer;
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

PoolHandler<Material> MaterialManager::getMaterialHandler(u32 id) const
{
    PoolHandler<Material> handler = mMaterials.getHandler(id);
    CHECK_MSG(handler.isValid(), "Invalid handler!");
    return handler;
}

const Material& MaterialManager::getMaterial(u32 id) const
{
    return getMaterialHandler(id).get();
}