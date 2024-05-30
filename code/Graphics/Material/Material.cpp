#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/RenderState.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Graphics/Model/Model.hpp"

void Material::internalInit(const MaterialData& materialData, u32 id)
{
    mMaterialData = materialData;
	mID = id;

    PoolHandler<Material> handler = GET_SYSTEM(MaterialManager).getMaterialHandler(mID);
    mShader->init(handler);
}

void Material::terminate()
{
}

void Material::enable() const
{
	PROFILER_CPU()
    GET_SYSTEM(GPUInterface).setFaceMode(mMaterialData.mCullFaceType);
}

void Material::disable() const
{
	PROFILER_CPU()
}