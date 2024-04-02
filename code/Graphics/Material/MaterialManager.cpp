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
    materialData.mSharedMaterialPropertiesBlockBuffer.set<MaterialPropertiesBlock>();
	
    mNoTextureMaterial = createMaterial(materialData);
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

PoolHandler<Material> MaterialManager::createMaterial(const MaterialData& materialData, MaterialRuntime* materialRuntime)
{
    PoolHandler<Material> handler = mMaterials.allocate();
    Material& material = mMaterials.get(handler);

    materialRuntime->init(handler);

    material.init(materialData, handler.getIndex(), materialRuntime);

    GET_SYSTEM(RenderSharedContext).initMaterialInstancePropertiesSharedBuffer(handler);

    return handler;
}

PoolHandler<Material> MaterialManager::createMaterial(const MaterialData& materialData)
{
    MaterialRuntime* materialRuntime = Memory::newObject<MaterialRuntime>();//OwnerPtr<MaterialRuntime>::newObject();
    PoolHandler<Material> handler = createMaterial(materialData, materialRuntime);
    return handler;
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