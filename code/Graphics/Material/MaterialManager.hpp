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
	std::map<std::string, OwnerRef<Texture>> mTexturesMap;
	std::map<std::string, OwnerRef<Material>> mMaterialsMap;
	OwnerRef<Material> mNoTextureMaterial;

public:
	~MaterialManager() override;

	void init();
	Ref<Texture> loadTexture(const std::string& path);
	Ref<Material> loadMaterial(const std::string& path);
	Ref<Material> loadNoTextureMaterial();
};