#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Material/Shader.hpp"

MaterialManager::~MaterialManager()
{
	//MAP_DELETE_CONTENT(mTexturesMap)
	//MAP_DELETE_CONTENT(mMaterialsMap)

	// if (mNoTextureMaterial)
	// {
	// 	DELETE(mNoTextureMaterial);
	// }

	Shader::freeStaticShaders();
}

void MaterialManager::init(){
	TRACE()}

Ptr<Texture> MaterialManager::loadTexture(const std::string& path)
{
	if (!MAP_CONTAINS(mTexturesMap, path))
	{
		OwnerPtr<Texture> texture = OwnerPtr<Texture>(NEW(Texture));
		texture.get().init(path);
		MAP_INSERT(mTexturesMap, path, texture);
	}

	return mTexturesMap.at(path);
}

Ptr<Material> MaterialManager::loadMaterial(const std::string& path)
{
	if (!MAP_CONTAINS(mMaterialsMap, path))
	{
		OwnerPtr<Material> material = OwnerPtr<Material>(NEW(Material));
		material.get().init();
		material.get().setTexture(loadTexture(path));
		material.get().setShader(Shader::getDefaultShader());
		MAP_INSERT(mMaterialsMap, path, material);
	}

	return mMaterialsMap.at(path);
}

Ptr<Material> MaterialManager::loadNoTextureMaterial()
{
	if (!mNoTextureMaterial)
	{
		mNoTextureMaterial = OwnerPtr<Material>(NEW(Material));
		mNoTextureMaterial.get().init();
		mNoTextureMaterial.get().setShader(Shader::getDefaultShader());
	}

	return mNoTextureMaterial;
}