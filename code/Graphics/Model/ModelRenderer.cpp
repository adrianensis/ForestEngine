#include "Graphics/Model/ModelRenderer.hpp"

#include "Graphics/Model/Model.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Core/EntityComponent/Entity.hpp"

void ModelRenderer::init(const ModelRendererData& data) 
{
    mModelRendererData = data;
}

void ModelRenderer::onComponentAdded() 
{
	const auto& meshes = mModelRendererData.mModel->getMeshInstances();
    FOR_LIST(it, meshes)
	{
        GPURenderItemData rendererData;
		rendererData.mMesh = (*it).mMesh;
		rendererData.mMeshInstanceMatrix = (*it).mMatrix;
		rendererData.mShader = mModelRendererData.mModel->getMeshShaders().at((*it).mMesh);
		rendererData.mGPUShaderStencilData = mModelRendererData.mGPUShaderStencilData;
		rendererData.mRenderPassIDs = mModelRendererData.mRenderPassIDs;

        EC::EntityPtr<EC::Entity> parent = getOwnerEntity();
        EC::ComponentPtr<MeshRenderer> renderer = ECManager.requestComponent<MeshRenderer>();
        renderer->init(rendererData);
    	ECManager.addComponent(parent, renderer);
	}
}

void ModelRenderer::onDestroy() 
{

}
