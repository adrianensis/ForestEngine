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
	mMaterials[mMaterialIDCounter].init(materialData, mMaterialIDCounter);
	mMaterialIDCounter++;
}

Ptr<const Texture> MaterialManager::loadTexture(const TextureData& textureData)
{
	if (!mTexturesMap.contains(textureData.mPath))
	{
        OwnerPtr<TextureImage> texture = OwnerPtr<TextureImage>::newObject();
        mTexturesMap.insert_or_assign(textureData.mPath, OwnerPtr<Texture>::moveCast(texture));
		
		Ptr<Texture> texturePtr = mTexturesMap.at(textureData.mPath);
        texturePtr->init(textureData);
	}

	return mTexturesMap.at(textureData.mPath);
}

Ptr<const TextureFont> MaterialManager::loadTextureFont(const TextureData& textureData)
{
	if (!mTexturesMap.contains(textureData.mPath))
	{
        OwnerPtr<TextureFont> texture = OwnerPtr<TextureFont>::newObject();
        mTexturesMap.insert_or_assign(textureData.mPath, OwnerPtr<Texture>::moveCast(texture));
		
		Ptr<Texture> texturePtr = mTexturesMap.at(textureData.mPath);
        texturePtr->init(textureData);
	}

	return Ptr<const TextureFont>::cast(mTexturesMap.at(textureData.mPath));
}

u32 MaterialManager::createMaterial(const MaterialData& materialData)
{
    u32 index = mMaterialIDCounter;
    if (mMaterials[index].getID() == 0)
    {
        Material& material = mMaterials.at(index);
        material.init(materialData, index);
        mMaterialIDCounter++;
    }

    return index;
}

const Material& MaterialManager::getMaterial(u32 index) const
{
    return mMaterials.at(index);
}