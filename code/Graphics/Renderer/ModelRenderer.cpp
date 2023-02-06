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
	const auto& meshes = mModel.get().mMeshes;
    FOR_LIST(it, meshes)
	{
		OwnerPtr<Renderer> renderer = OwnerPtr<Renderer>(Memory::newObject<Renderer>());
		renderer.get().init();

		renderer.get().mMesh = *it;
		renderer.get().mMaterial = renderer.get().mMesh.get().mMaterial;
		renderer.get().mIsInstanced = mIsInstanced;
		renderer.get().mStencilValue = mStencilValue;
		renderer.get().mIsStencilMask = mIsStencilMask;
		mGameObject.get().addComponent<Renderer>(renderer);
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