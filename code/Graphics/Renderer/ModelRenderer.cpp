#include "Graphics/Renderer/ModelRenderer.hpp"

#include "Graphics/Model/Model.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Scene/Module.hpp"

void ModelRenderer::init(ModelRendererData& data) 
{
    ComponentWithData::init(data);
}

void ModelRenderer::onComponentAdded() 
{
	const auto& meshes = getComponentData().mModel->mMeshes;
    FOR_LIST(it, meshes)
	{
        RendererData rendererData;
		rendererData.mMesh = *it;
		rendererData.mMaterial = (*it)->mMaterial;
		rendererData.mIsInstanced = getComponentData().mIsInstanced;
		rendererData.mStencilData = getComponentData().mStencilData;

        mGameObject->createComponent<MeshRenderer>(rendererData);
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