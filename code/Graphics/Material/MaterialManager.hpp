#pragma once

#include "Core/Module.hpp"

#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"

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
	std::unordered_map<std::string, SharedPtr<Texture>> mTexturesMap;
    std::unordered_map<u32, SharedPtr<Material>> mMaterials;
	SharedPtr<Material> mNoTextureMaterial;
};
