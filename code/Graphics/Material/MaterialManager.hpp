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
    Handler createMaterial(const MaterialData& materialData);
    const Material& getMaterial(const Handler& handler) const;

private:
    inline constexpr static const u32 smMaxMaterials = 3000;
    u32 mMaterialIDCounter = 0;
	std::unordered_map<std::string, OwnerPtr<Texture>> mTexturesMap;
    std::array<Material, smMaxMaterials> mMaterials;

    Handler mNoTextureMaterial;

public:
    GET(NoTextureMaterial)
};
