#pragma once

#include "Core/Module.hpp"
#include "Core/Singleton.hpp"

class Material;
class Texture;

class MaterialManager: public EngineSystem
{
	GENERATE_METADATA(MaterialManager)

public:
    ~MaterialManager() override;
    
    void init();
    Ptr<const Texture> loadTexture(const std::string& path);
    Ptr<Material> createMaterial();
    Ptr<Material> createMaterialWithTexture(const std::string& path);
    Ptr<const Material> getMaterial(u32 index) const;
    Ptr<const Material> getNoTextureMaterial();

private:
    u32 mMaterialIDCounter = 0;
	std::map<std::string, OwnerPtr<Texture>> mTexturesMap;
    std::map<u32, OwnerPtr<Material>> mMaterials;
	OwnerPtr<Material> mNoTextureMaterial;
};
