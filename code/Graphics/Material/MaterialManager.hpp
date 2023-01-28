#pragma once

#include "Core/Module.hpp"
#include "Core/Singleton.hpp"

class Material;
class Texture;

class MaterialManager: public ObjectBase, public Singleton<MaterialManager>
{
	GENERATE_METADATA(MaterialManager)

public:
    ~MaterialManager() override;
    
    void init();
    Ptr<const Texture> loadTexture(const std::string& path);
    Ptr<const Material> loadMaterial(const std::string& path);
    Ptr<const Material> loadNoTextureMaterial();

private:
	std::map<std::string, OwnerPtr<Texture>> mTexturesMap;
	std::map<std::string, OwnerPtr<Material>> mMaterialsMap;
	OwnerPtr<Material> mNoTextureMaterial;
};
