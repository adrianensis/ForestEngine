#pragma once

#include "Core/Module.hpp"
#include "Core/Singleton.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"

class Material;
class Texture;

class MaterialManager: public ObjectBase, public Singleton<MaterialManager>
{
	GENERATE_METADATA(MaterialManager)
private:
	std::map<std::string, OwnerPtr<Texture>> mTexturesMap;
	std::map<std::string, OwnerPtr<Material>> mMaterialsMap;
	OwnerPtr<Material> mNoTextureMaterial;

public:
	~MaterialManager() override;

	void init();
	Ptr<Texture> loadTexture(const std::string& path);
	Ptr<Material> loadMaterial(const std::string& path);
	Ptr<Material> loadNoTextureMaterial();
};