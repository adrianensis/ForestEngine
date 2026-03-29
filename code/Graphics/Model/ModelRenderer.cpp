#include "Graphics/Model/ModelRenderer.hpp"

#include "Graphics/Model/Model.hpp"
#include "Scene/GameObject.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Core/EntityComponent/Entity.hpp"

void ModelRenderer::init(const ModelRendererData& data) 
{
    mModelRendererData = data;
}

void ModelRenderer::onECComponentAdded() 
{
	const auto& meshes = mModelRendererData.mModel->getMeshInstances();
    FOR_LIST(it, meshes)
	{
        GPURenderItemData rendererData;
		rendererData.mMesh = (*it).mMesh;
		rendererData.mMeshInstanceMatrix = (*it).mMatrix;
		rendererData.mShader = mModelRendererData.mModel->getMeshShaders().at((*it).mMesh);
		rendererData.mGPUDepthStencilData = mModelRendererData.mGPUDepthStencilData;
		rendererData.mRenderPassIDs = mModelRendererData.mRenderPassIDs;

        EC::Entity* parent = getOwnerEntity();
        getOwnerGameObject()->getSystemsDI().getSystem<EC::EntityComponentManager>()->requestComponent<MeshRenderer>(parent, [&](auto* component)
		{
			component->init(rendererData);
		});
	}
}

void ModelRenderer::onDestroy() 
{

}
