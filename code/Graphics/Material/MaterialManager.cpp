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

Ref<Texture> MaterialManager::loadTexture(const std::string& path)
{
	if (!MAP_CONTAINS(mTexturesMap, path))
	{
		OwnerRef<Texture> texture = OwnerRef<Texture>(NEW(Texture));
		texture.get().init(path);
		MAP_INSERT(mTexturesMap, path, texture);
	}

	return mTexturesMap.at(path);
}

Ref<Material> MaterialManager::loadMaterial(const std::string& path)
{
	if (!MAP_CONTAINS(mMaterialsMap, path))
	{
		OwnerRef<Material> material = OwnerRef<Material>(NEW(Material));
		material.get().init();
		material.get().setTexture(loadTexture(path));
		material.get().setShader(Shader::getDefaultShader());
		MAP_INSERT(mMaterialsMap, path, material);
	}

	return mMaterialsMap.at(path);
}

Ref<Material> MaterialManager::loadNoTextureMaterial()
{
	if (!mNoTextureMaterial)
	{
		mNoTextureMaterial = OwnerRef<Material>(NEW(Material));
		mNoTextureMaterial.get().init();
		mNoTextureMaterial.get().setShader(Shader::getDefaultShader());
	}

	return mNoTextureMaterial;
}