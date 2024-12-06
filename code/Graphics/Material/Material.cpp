#include "Graphics/Material/Material.hpp"

void Material::init(const MaterialData& materialData, u32 id)
{
    mMaterialData = materialData;
	mID = id;
}

void Material::terminate()
{
}