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
    Ptr<const Material> createMaterial();
    Ptr<const Material> createMaterial(const MaterialData& materialData);
    Ptr<const Material> getMaterial(u32 index) const;

private:
    u32 mMaterialIDCounter = 0;
	std::unordered_map<std::string, OwnerPtr<Texture>> mTexturesMap;
    std::unordered_map<u32, OwnerPtr<Material>> mMaterials;
	OwnerPtr<Material> mNoTextureMaterial;
	OwnerPtr<Material> mNoTextureMaterialVertexColor;

public:
    CGET(NoTextureMaterial)
    CGET(NoTextureMaterialVertexColor)
};
