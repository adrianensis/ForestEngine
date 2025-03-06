#include "Graphics/Renderer/ModelRenderer.hpp"

#include "Graphics/Model/Model.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "Core/EntityComponent/ComponentsManager.hpp"
#include "Core/EntityComponent/EntityHandler.hpp"

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
		rendererData.mShader = mModelRendererData.mModel->getMeshShaders().at((*it).mMesh);
		rendererData.mGPUShaderStencilData = mModelRendererData.mGPUShaderStencilData;
		rendererData.mRenderPassIDs = mModelRendererData.mRenderPassIDs;

        EntityHandler parent = getOwnerEntity();
        TComponentHandler<MeshRenderer> renderer = ComponentsManager::getInstance().requestComponent<MeshRenderer>();
        renderer->init(rendererData);
		parent->addComponent(renderer);
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