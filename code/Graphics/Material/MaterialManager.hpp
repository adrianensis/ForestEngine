#pragma once

#include "Core/Module.hpp"

class Material;
class Texture;

class MaterialManager: public EngineSystem
{
	GENERATE_METADATA(MaterialManager)

public:
    ~MaterialManager() override;
    
    void init();
    Ptr<const Texture> loadTexture(const std::string& path, bool createMipMap);
    Ptr<Material> createMaterial();
    Ptr<Material> createMaterialWithTexture(const std::string& path, bool createMipMap);
    Ptr<const Material> getMaterial(u32 index) const;
    Ptr<const Material> getNoTextureMaterial();

private:
    u32 mMaterialIDCounter = 0;
	std::unordered_map<std::string, OwnerPtr<Texture>> mTexturesMap;
    std::unordered_map<u32, OwnerPtr<Material>> mMaterials;
	OwnerPtr<Material> mNoTextureMaterial;
};
