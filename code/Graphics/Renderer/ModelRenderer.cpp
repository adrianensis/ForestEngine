#include "Graphics/Renderer/ModelRenderer.hpp"

#include "Graphics/Model/Model.hpp"
#include "Graphics/GPU/GPUMesh.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Scene/Module.hpp"

void ModelRenderer::init(const ModelRendererData& data) 
{
    mModelRendererData = data;
}

void ModelRenderer::onComponentAdded() 
{
	const auto& meshes = mModelRendererData.mModel->getMeshInstances();
    FOR_LIST(it, meshes)
	{
        RendererData rendererData;
		rendererData.mMesh = (*it).mMesh;
		rendererData.mMeshInstanceMatrix = (*it).mMatrix;
		rendererData.mMaterial = mModelRendererData.mModel->getMeshMaterials().at((*it).mMesh);
		rendererData.mStencilData = mModelRendererData.mStencilData;
		rendererData.mRenderPassIDs = mModelRendererData.mRenderPassIDs;

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