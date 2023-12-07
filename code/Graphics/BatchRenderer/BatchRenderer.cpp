#include "Graphics/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Light/Light.hpp"
#include "Scene/Module.hpp"
#include "Graphics/ShaderBuilder/ShaderBuilder.hpp"

void BatchRenderer::init(const BatchData& batchData)
{
	mBatchData = batchData;
	mMeshBatcher.init(mBatchData);

    const GPUVertexBuffersLayout& gpuVertexBuffersLayout = mMeshBatcher.getGPUVertexBuffersLayout();
    
    ShaderBuilder sbVert;
    sbVert.createVertexShader(gpuVertexBuffersLayout, mBatchData.mMaterial);
    ShaderBuilder sbFrag;
    sbFrag.createFragmentShader(gpuVertexBuffersLayout, mBatchData.mMaterial);

    mShader = OwnerPtr<GPUShader>::newObject();
    std::string stringShderVert = sbVert.getCode();
    LOG(stringShderVert);
    std::string stringShderFrag = sbFrag.getCode();
    LOG(stringShderFrag);
    mShader->initFromFileContents(stringShderVert, stringShderFrag);
    
    mShader->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).mGlobalMatricesBuffer);
	GET_SYSTEM(GPUSharedContext).mGlobalMatricesBuffer.resize(1);

    if(mBatchData.mMaterial->getMaterialData().mReceiveLight)
    {
        mShader->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).mLightsBuffer);
    }
    
    mMeshBatcher.bindUniforms(mShader);
}

void BatchRenderer::render()
{
	PROFILER_CPU()

	if (!mRenderers.empty())
	{
        enable();

		if(shouldRegenerateBuffers())
		{
			processRenderers();
		}

        mMeshBatcher.drawCall();

        disable();
	}
}

void BatchRenderer::enable()
{
    mShader->enable();
    mMeshBatcher.enable();
    mBatchData.mMaterial->bind(mShader, mBatchData.mIsWorldSpace, mBatchData.mIsInstanced, mBatchData.mMesh);

    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        mMeshBatcher.updateBoneTransforms();
    }

    if(mBatchData.mStencilData.mUseStencil)
    {
        GET_SYSTEM(GPUInterface).enableStencil(mBatchData.mStencilData.mMaskStencilValue, mBatchData.mStencilData.mStencilFunction, mBatchData.mStencilData.mStencilPassOp);
    }
}

void BatchRenderer::disable()
{
    if(mBatchData.mStencilData.mUseStencil)
    {
        GET_SYSTEM(GPUInterface).disableStencil();
    }

    mShader->disable();
    mMeshBatcher.disable();
}

void BatchRenderer::addRenderer(Ptr<MeshRenderer> renderer)
{
	mRenderers.push_back(renderer);
	renderer->setBatchRenderer(getPtrToThis());

	mNewRendererAdded = true;
}

void BatchRenderer::processRenderers()
{
	PROFILER_CPU()

	u32 newSize = mRenderers.size();
	mMeshBatcher.resize(newSize);
	
    FOR_LIST(it, mRenderers)
	{
		Ptr<MeshRenderer> renderer = *it;
        if (shouldRemoveRenderer(renderer))
		{
            PROFILER_BLOCK_CPU("remove renderer");
			internalRemoveRenderer(it);
		}
		else
		{
            PROFILER_BLOCK_CPU("update");
			renderer->update();
		    addToVertexBuffer(renderer);
		}
	}

    mNewRendererAdded = false;
    mRegenerateBuffersRequested = false;
}

bool BatchRenderer::shouldRemoveRenderer(Ptr<const MeshRenderer> renderer)
{
	bool toRemove = false;

	if(renderer.isValid())
	{
		if (renderer->getIsPendingToBeDestroyed())
        {
            toRemove = true;
        }
	}
	else
	{
		toRemove = true;
	}

	return toRemove;
}

void BatchRenderer::internalRemoveRenderer(std::list<Ptr<MeshRenderer>>::iterator& it)
{
	PROFILER_CPU()

	Ptr<MeshRenderer> renderer = *it;
	if(renderer)
	{
		renderer->setBatchRenderer(Ptr<BatchRenderer>());

        if (!mBatchData.mIsWorldSpace)
        {
            if (renderer->getIsPendingToBeDestroyed())
            {
                renderer->finallyDestroy();
            }
        }
	}

    it->invalidate();

    mRegenerateBuffersRequested = true;

	it = mRenderers.erase(it);
	--it; // go back to the previous it, so the FOR LOOP can do ++it with no problem
}

void BatchRenderer::addToVertexBuffer(Ptr<MeshRenderer> renderer)
{
	PROFILER_CPU()

    const Matrix4& rendererModelMatrix = renderer->getRendererModelMatrix();
    mMeshBatcher.addInstance(rendererModelMatrix, renderer->getMeshInstance());
}

bool BatchRenderer::shouldRegenerateBuffers() const
{
    // TODO: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mNewRendererAdded || !mBatchData.mIsStatic || mRegenerateBuffersRequested;
}
