#include "Graphics/Renderer/ModelRenderer.hpp"

#include "Graphics/Model/Model.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Scene/Module.hpp"

void ModelRenderer::init() 
{

}

void ModelRenderer::onComponentAdded() 
{
	const std::vector<OwnerPtr<Mesh>>&  meshes = mModel.get().getMeshes();

	FOR_LIST(it, meshes)
	{
		Renderer *renderer = NEW(Renderer);
		renderer->init();

		renderer->setMesh(*it);
		renderer->setMaterial(MaterialManager::getInstance().loadMaterial((*it).get().getMaterialPath()));
		renderer->setIsInstanced(mIsInstanced);
		renderer->setStencilValue(mStencilValue);
		renderer->setIsStencilMask(mIsStencilMask);
		getGameObject()->addComponent<Renderer>(renderer);
	}
}

void ModelRenderer::onDestroy() 
{

}

IMPLEMENT_SERIALIZATION(ModelRenderer)
{

}

IMPLEMENT_DESERIALIZATION(ModelRenderer)
{

}