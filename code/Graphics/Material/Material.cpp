#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

void Material::internalInit(const MaterialData& materialData, u32 id)
{
    mMaterialData = materialData;
	mID = id;

    PoolHandler<Material> handler = GET_SYSTEM(MaterialManager).getMaterialHandler(mID);
}

void Material::terminate()
{
}