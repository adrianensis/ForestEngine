#include "Graphics/Renderer/ModelRenderer.hpp"

#include "Graphics/Model/Model.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Scene/Module.hpp"

void ModelRenderer::init() 
{

}

void ModelRenderer::onComponentAdded() 
{
	const std::vector<OwnerPtr<Mesh>>&  meshes = mModel.get().mMeshes;

	FOR_LIST(it, meshes)
	{
		Renderer *renderer = NEW(Renderer);
		renderer->init();

		renderer->mMesh = (*it);
		renderer->mMaterial = (MaterialManager::getInstance().loadMaterial((*it).get().mMaterialPath));
		renderer->mIsInstanced = (mIsInstanced);
		renderer->mStencilValue = (mStencilValue);
		renderer->mIsStencilMask = (mIsStencilMask);
		mGameObject->addComponent<Renderer>(renderer);
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