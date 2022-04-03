#pragma once

#include "Core/Module.hpp"
#include "Core/Singleton.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Material/Shader.hpp"
#endif

class Material;
class Texture;

class MaterialManager: public ObjectBase, public Singleton<MaterialManager>
{
	GENERATE_METADATA(MaterialManager)

public:
	CPP ~MaterialManager() override
	{
		//MAP_DELETE_CONTENT(mTexturesMap)
		//MAP_DELETE_CONTENT(mMaterialsMap)

		// if (mNoTextureMaterial)
		// {
		// 	DELETE(mNoTextureMaterial);
		// }

		Shader::freeStaticShaders();
	}

	CPP void init()
	{
		TRACE()
	}

	CPP Ptr<Texture> loadTexture(const std::string& path)
	{
		if (!MAP_CONTAINS(mTexturesMap, path))
		{
			OwnerPtr<Texture> texture = OwnerPtr<Texture>(NEW(Texture));
			texture.get().init(path);
			MAP_INSERT(mTexturesMap, path, texture);
		}

		return mTexturesMap.at(path);
	}

	CPP Ptr<Material> loadMaterial(const std::string& path)
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

	CPP Ptr<Material> loadNoTextureMaterial()
	{
		if (!mNoTextureMaterial)
		{
			mNoTextureMaterial = OwnerPtr<Material>(NEW(Material));
			mNoTextureMaterial.get().init();
			mNoTextureMaterial.get().setShader(Shader::getDefaultShader());
		}

		return mNoTextureMaterial;
	}

private:
	std::map<std::string, OwnerPtr<Texture>> mTexturesMap;
	std::map<std::string, OwnerPtr<Material>> mMaterialsMap;
	OwnerPtr<Material> mNoTextureMaterial;
};