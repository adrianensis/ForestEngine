#pragma once

// CPP_IGNORE

#include "Core/Module.hpp"
#include "Core/Singleton.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"

class MaterialManager: public ObjectBase, public Singleton<MaterialManager>
{
	GENERATE_METADATA(MaterialManager)
private:
	std::map<std::string, class Texture *> mTexturesMap;
	std::map<std::string, class Material *> mMaterialsMap;
	Material* mNoTextureMaterial = nullptr;

public:
	~MaterialManager() override;

	void init();
	Texture *loadTexture(const std::string& path);
	Material *loadMaterial(const std::string& path);
	Material *loadNoTextureMaterial();
};