#include "Graphics/Renderer/ModelRenderer.hpp"

#include "Graphics/Model/Model.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Scene/Module.hpp"

void ModelRenderer::init(const ModelRendererData& data) 
{
    mModelRendererData = data;
}

void ModelRenderer::onComponentAdded() 
{
	const auto& meshes = mModelRendererData.mModel->getMeshes();
    FOR_LIST(it, meshes)
	{
        RendererData rendererData;
		rendererData.mMesh = *it;
		rendererData.mMaterial = mModelRendererData.mModel->getMeshMaterials().at(*it);
		rendererData.mIsInstanced = mModelRendererData.mIsInstanced;
		rendererData.mStencilData = mModelRendererData.mStencilData;

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