#ifndef MATERIALMANAGER_HPP
#define MATERIALMANAGER_HPP

#include "Core/Module.hpp"
#include "Core/Singleton.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"


class Material;
class Texture;

class MaterialManager: public ObjectBase, public Singleton<MaterialManager>
{
	GENERATE_METADATA(MaterialManager)

public:
    ~MaterialManager() override;
    
    void init();
    Ptr<Texture> loadTexture(const String& path);
    Ptr<Material> loadMaterial(const String& path);
    Ptr<Material> loadNoTextureMaterial();

private:
	std::map<String, OwnerPtr<Texture>> mTexturesMap;
	std::map<String, OwnerPtr<Material>> mMaterialsMap;
	OwnerPtr<Material> mNoTextureMaterial;
};

#endif