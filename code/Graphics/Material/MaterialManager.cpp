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
    MaterialData materialData;
    materialData.mAlphaEnabled = true;
    materialData.mUseColorAsTint = true;
    mNoTextureMaterial = OwnerPtr<Material>::newObject();
	mNoTextureMaterial->init(materialData, mMaterialIDCounter);
	mMaterialIDCounter++;

    MaterialData materialDataVertexColor;
    materialDataVertexColor.mAlphaEnabled = true;
    materialDataVertexColor.mUseColorAsTint = true;
    materialDataVertexColor.mUseVertexColor = true;
    mNoTextureMaterialVertexColor = OwnerPtr<Material>::newObject();
	mNoTextureMaterialVertexColor->init(materialDataVertexColor, mMaterialIDCounter);
	mMaterialIDCounter++;
}

Ptr<const Texture> MaterialManager::loadTexture(const TextureData& textureData, bool isFont)
{
	if (!MAP_CONTAINS(mTexturesMap, textureData.mPath))
	{
        if(isFont)
        {
            OwnerPtr<TextureFont> texture = OwnerPtr<TextureFont>::newObject();
		    MAP_INSERT(mTexturesMap, textureData.mPath, OwnerPtr<Texture>::moveCast(texture));
        }
        else
        {
            OwnerPtr<TextureImage> texture = OwnerPtr<TextureImage>::newObject();
		    MAP_INSERT(mTexturesMap, textureData.mPath, OwnerPtr<Texture>::moveCast(texture));
        }
		
		Ptr<Texture> texture = mTexturesMap.at(textureData.mPath);
        texture->init(textureData);
	}

	return mTexturesMap.at(textureData.mPath);
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

Ptr<const MaterialFont> MaterialManager::createMaterialFont(const MaterialData& materialData)
{
    u32 index = mMaterialIDCounter;
    if (!MAP_CONTAINS(mMaterials, index))
    {
        OwnerPtr<MaterialFont> materialFont = OwnerPtr<MaterialFont>::newObject();
        MAP_INSERT(mMaterials, index, OwnerPtr<Material>::moveCast(materialFont));
        Ptr<Material> material = mMaterials.at(index);
        material->init(materialData, index);
        mMaterialIDCounter++;
    }

    return Ptr<const MaterialFont>::cast(mMaterials[index]);
}

Ptr<const Material> MaterialManager::getMaterial(u32 index) const
{
    return mMaterials.at(index);
}