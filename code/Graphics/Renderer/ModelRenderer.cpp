#include "Graphics/Renderer/ModelRenderer.hpp"

#include "Graphics/Model/Model.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Scene/Module.hpp"
#include "Core/ECS/ComponentsManager.hpp"

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
		rendererData.mShaderStencilData = mModelRendererData.mShaderStencilData;
		rendererData.mRenderPassIDs = mModelRendererData.mRenderPassIDs;

        TEntityHandler<GameObject> gameObjectParent = getOwnerEntity();
        TComponentHandler<MeshRenderer> renderer = ComponentsManager::getInstance().requestComponent<MeshRenderer>();
        renderer->init(rendererData);
		gameObjectParent->addComponent(renderer);
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