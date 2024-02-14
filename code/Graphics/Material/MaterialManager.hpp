#pragma once

#include "Core/Module.hpp"
#include "Core/Systems/System.hpp"
#include "Graphics/Material/Material.hpp"

class MaterialManager: public System
{
	GENERATE_METADATA(MaterialManager)

public:
    void init();
    Ptr<const Texture> loadTexture(const TextureData& textureData);
    Ptr<const TextureFont> loadTextureFont(const TextureData& textureData);
    u32 createMaterial(const MaterialData& materialData);
    const Material& getMaterial(u32 index) const;

private:
    inline constexpr static const u32 smMaxMaterials = 3000;
    u32 mMaterialIDCounter = 0;
	std::unordered_map<std::string, OwnerPtr<Texture>> mTexturesMap;
    std::array<Material, smMaxMaterials> mMaterials;

    u32 mNoTextureMaterial = 0;

public:
    GET(NoTextureMaterial)
};
