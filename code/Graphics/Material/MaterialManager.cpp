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

	// reserve index 0 and 1 for no textured material
    mNoTextureMaterial = OwnerPtr<Material>::newObject();
	mNoTextureMaterial->init(MaterialData(), mMaterialIDCounter);
	mMaterialIDCounter++;

    MaterialData materialData;
    materialData.mUseVertexColor = true;
    mNoTextureMaterialVertexColor = OwnerPtr<Material>::newObject();
	mNoTextureMaterialVertexColor->init(materialData, mMaterialIDCounter);
	mMaterialIDCounter++;
}

Ptr<const Texture> MaterialManager::loadTexture(const std::string& path, bool createMipMap)
{
	if (!MAP_CONTAINS(mTexturesMap, path))
	{
		MAP_INSERT(mTexturesMap, path, OwnerPtr<Texture>::newObject());
		Ptr<Texture> texture = mTexturesMap.at(path);
		texture->init(path, createMipMap);
	}

	return mTexturesMap.at(path);
}

Ptr<const Material> MaterialManager::createMaterial()
{
    return createMaterial(MaterialData());
}

Ptr<const Material> MaterialManager::createMaterial(const MaterialData& materialData)
{
    u32 index = mMaterialIDCounter;
    if (!MAP_CONTAINS(mMaterials, index))
    {
        MAP_INSERT(mMaterials, index, OwnerPtr<Material>::newObject());
        Ptr<Material> material = mMaterials.at(index);
        material->init(materialData, index);
        mMaterialIDCounter++;
    }

    return mMaterials[index];
}

Ptr<const Material> MaterialManager::createMaterialWithTexture(const std::string &path, bool createMipMap)
{
    MaterialData materialData;
    materialData.mTextures[(u32)TextureType::BASE_COLOR] = loadTexture(path, createMipMap);

    Ptr<const Material> material = createMaterial(materialData);
    return material;
}

Ptr<const Material> MaterialManager::getMaterial(u32 index) const
{
    return mMaterials.at(index);
}