#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"

MaterialManager::~MaterialManager() 
{

}

void MaterialManager::init()
{
	TRACE()

	// reserve index 0 for no textured material
	mMaterialIDCounter++;
}

Ptr<const Texture> MaterialManager::loadTexture(const std::string& path, bool createMipMap)
{
	if (!MAP_CONTAINS(mTexturesMap, path))
	{
		MAP_INSERT(mTexturesMap, path, SharedPtr<Texture>::newObject());
		Ptr<Texture> texture = mTexturesMap.at(path);
		texture->init(path, createMipMap);
	}

	return mTexturesMap.at(path);
}

Ptr<const Material> MaterialManager::getNoTextureMaterial()
{
	if (!mNoTextureMaterial)
	{
		mNoTextureMaterial = SharedPtr<Material>::newObject();
		mNoTextureMaterial->init(0);
	}

	return mNoTextureMaterial;
}

Ptr<Material> MaterialManager::createMaterial()
{
    u32 index = mMaterialIDCounter;
    if (!MAP_CONTAINS(mMaterials, index))
    {
        MAP_INSERT(mMaterials, index, SharedPtr<Material>::newObject());
        Ptr<Material> material = mMaterials.at(index);
        material->init(index);
        mMaterialIDCounter++;
    }

    return mMaterials[index];
}

Ptr<Material> MaterialManager::createMaterialWithTexture(const std::string &path, bool createMipMap)
{
    Ptr<Material> material = createMaterial();
    material->mTextures[(u32)TextureType::BASE_COLOR] = loadTexture(path, createMipMap);
    return material;
}

Ptr<const Material> MaterialManager::getMaterial(u32 index) const
{
    return mMaterials.at(index);
}