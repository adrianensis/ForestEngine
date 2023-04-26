#include "Graphics/Renderer/ModelRenderer.hpp"

#include "Graphics/Model/Model.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Scene/Module.hpp"

void ModelRenderer::init() 
{

}

void ModelRenderer::onComponentAdded() 
{
	const auto& meshes = mModel->mMeshes;
    FOR_LIST(it, meshes)
	{
		SharedPtr<Renderer> renderer = SharedPtr<Renderer>::newObject();
		renderer->init();

		renderer->mMesh = *it;
		renderer->mMaterial = renderer->mMesh->mMaterial;
		renderer->mIsInstanced = mIsInstanced;
		renderer->mStencilValue = mStencilValue;
		renderer->mIsStencilMask = mIsStencilMask;
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