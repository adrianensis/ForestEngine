#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"

MaterialManager::~MaterialManager() 
{


	Shader::freeStaticShaders();
}

void MaterialManager::init()
{
	TRACE()

	// reserve index 0 for no textured material
	mMaterialIDCounter++;
}

Ptr<const Texture> MaterialManager::loadTexture(const std::string& path)
{
	if (!MAP_CONTAINS(mTexturesMap, path))
	{
		OwnerPtr<Texture> texture = OwnerPtr<Texture>(Memory::newObject<Texture>());
		texture.get().init(path);
		MAP_INSERT(mTexturesMap, path, texture);
	}

	return mTexturesMap.at(path);
}

Ptr<const Material> MaterialManager::getNoTextureMaterial()
{
	if (!mNoTextureMaterial)
	{
		mNoTextureMaterial = OwnerPtr<Material>(Memory::newObject<Material>());
		mNoTextureMaterial.get().init(0);
		mNoTextureMaterial.get().mShader = (Shader::getDefaultShader());
	}

	return mNoTextureMaterial;
}

Ptr<Material> MaterialManager::createMaterial()
{
    u32 index = mMaterialIDCounter;
    if (!MAP_CONTAINS(mMaterials, index))
    {
        OwnerPtr<Material> material = OwnerPtr<Material>(Memory::newObject<Material>());
        material.get().init(index);
        material.get().mShader = (Shader::getDefaultShader());
        MAP_INSERT(mMaterials, index, material);
        mMaterialIDCounter++;
    }

    return mMaterials[index];
}

Ptr<Material> MaterialManager::createMaterialWithTexture(const std::string &path)
{
    Ptr<Material> material = createMaterial();
    material.get().mTextures[(u32)TextureType::DIFFUSE] = loadTexture(path);
    return material;
}

Ptr<const Material> MaterialManager::getMaterial(u32 index) const
{
    return mMaterials.at(index);
}