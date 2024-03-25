#include "Graphics/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/RenderSharedContext.hpp"
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
    
    mShader = ShaderUtils::createShader(gpuVertexBuffersContainer, gpuSharedBuffersContainer, mBatchData.mMaterial.get());
    
    bindSharedBuffers();
}

void BatchRenderer::terminate()
{
    mMeshBatcher.terminate();
}

void BatchRenderer::bindSharedBuffers()
{
    mShader->bindSharedBuffer(GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData));

    mShader->bindSharedBuffer(GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices));

    if(mBatchData.mMaterial->getMaterialData().mReceiveLight)
    {
        mShader->bindSharedBuffer(GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mLightsData));
    }

    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        mShader->bindSharedBuffer(mMeshBatcher.getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices));
    }
    
    mShader->bindSharedBuffer(GET_SYSTEM(RenderSharedContext).getMaterialPropertiesGPUSharedBuffer(mBatchData.mMaterial));
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
    mBatchData.mMaterial->enable();

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

    mBatchData.mMaterial->disable();
    mMeshBatcher.disable();
    mShader->disable();
}

void BatchRenderer::onAddRenderer(Ptr<MeshRenderer> renderer)
{
	mRenderers.push_back(renderer);
	mRegenerateBuffersRequested = true;
}

void BatchRenderer::onRemoveRenderer(Ptr<MeshRenderer> renderer)
{
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
        mMeshBatcher.addInstanceData(mRenderers[i]);
    }

    mRegenerateBuffersRequested = false;
}

bool BatchRenderer::shouldRegenerateBuffers() const
{
    // TODO: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mRegenerateBuffersRequested || !mBatchData.mIsStatic;
}
