#include "Graphics/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Light/Light.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Material/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"

void BatchRenderer::init(const BatchData& batchData)
{
	mBatchData = batchData;
	mMeshBatcher.init(mBatchData);

    const GPUVertexBuffersContainer& gpuVertexBuffersContainer = mMeshBatcher.getGPUVertexBuffersContainer();
    const GPUSharedBuffersContainer& gpuSharedBuffersContainer = mMeshBatcher.getGPUSharedBuffersContainer();
    
    mShader = ShaderUtils::createShader(gpuVertexBuffersContainer, gpuSharedBuffersContainer, GET_SYSTEM(MaterialManager).getMaterial(mBatchData.mMaterialId));
    
    bindSharedBuffers();
}

void BatchRenderer::terminate()
{
    mMeshBatcher.terminate();
}

void BatchRenderer::bindSharedBuffers()
{
    mShader->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData));

    mShader->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices));

    if(GET_SYSTEM(MaterialManager).getMaterial(mBatchData.mMaterialId).getMaterialData().mReceiveLight)
    {
        mShader->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mLightsData));
    }

    if(GET_SYSTEM(MaterialManager).getMaterial(mBatchData.mMaterialId).getMaterialData().mIsSkinned)
    {
        mShader->bindSharedBuffer(mMeshBatcher.getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices));
    }
    
    mShader->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(DefaultMaterialInstancedPropertiesGPUData::smDefaultInstancedPropertiesSharedBufferData));
}

void BatchRenderer::render()
{
	PROFILER_CPU()

	if (!mRenderers.empty())
	{
        enable();

		if(shouldRegenerateBuffers())
		{
			updateBuffers();
		}

        mMeshBatcher.drawCall();

        disable();
	}
}

void BatchRenderer::enable()
{
    mShader->enable();
    mMeshBatcher.enable();
    GET_SYSTEM(MaterialManager).getMaterial(mBatchData.mMaterialId).enable();

    if(GET_SYSTEM(MaterialManager).getMaterial(mBatchData.mMaterialId).getMaterialData().mIsSkinned)
    {
        mMeshBatcher.updateBoneTransforms();
    }

    if(mBatchData.mStencilData.mUseStencil)
    {
        GET_SYSTEM(GPUInterface).enableStencil(mBatchData.mStencilData.mStencilValue, mBatchData.mStencilData.mStencilFunction, mBatchData.mStencilData.mStencilPassOp);
    }
}

void BatchRenderer::disable()
{
    if(mBatchData.mStencilData.mUseStencil)
    {
        GET_SYSTEM(GPUInterface).disableStencil();
    }

    GET_SYSTEM(MaterialManager).getMaterial(mBatchData.mMaterialId).disable();
    mMeshBatcher.disable();
    mShader->disable();
}

void BatchRenderer::addRenderer(Ptr<MeshRenderer> renderer)
{
	mRenderers.push_back(renderer);
	renderer->setBatchRenderer(getPtrToThis());

	mRegenerateBuffersRequested = true;
}

void BatchRenderer::updateBuffers()
{
	PROFILER_CPU()
    
    std::vector<Ptr<MeshRenderer>> newList;
    FOR_ARRAY(i, mRenderers)
    {
        PROFILER_BLOCK_CPU("remove");

        Ptr<MeshRenderer> renderer = mRenderers[i];
        if(renderer.isValid())
        {
            newList.push_back(renderer);
        }
    }

    mRenderers.clear();
    mRenderers = newList;

    u32 newSize = mRenderers.size();
    mMeshBatcher.resize(newSize);

    FOR_ARRAY(i, mRenderers)
    {
        Ptr<MeshRenderer> renderer = mRenderers[i];
        mMeshBatcher.addInstanceData(renderer->getGPUInstanceSlot(), renderer->getMeshInstance());
    }

    mRegenerateBuffersRequested = false;
}

bool BatchRenderer::shouldRegenerateBuffers() const
{
    // TODO: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mRegenerateBuffersRequested || !mBatchData.mIsStatic;
}
