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
#include "Graphics/Shader/ShaderBuilder/ShaderBuilder.hpp"
#include "Graphics/Shader/ShaderUtils.hpp"

void BatchRenderer::init(const BatchData& batchData)
{
	mBatchData = batchData;
	mMeshBatcher.init(mBatchData);

    const GPUBuffersLayout& gpuBuffersLayout = mMeshBatcher.getGPUBuffersLayout();
    
    mShader = ShaderUtils::createShader(gpuBuffersLayout, mBatchData.mMaterial);
    
    bindSharedBuffers();
}

void BatchRenderer::terminate()
{
    mMeshBatcher.terminate();
}

void BatchRenderer::bindSharedBuffers()
{
    mShader->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).mGlobalDataBuffer);

    mShader->bindSharedBuffer(mMeshBatcher.getGPUBuffersLayout().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices));

    if(mBatchData.mMaterial->getMaterialData().mReceiveLight)
    {
        mShader->bindSharedBuffer(GET_SYSTEM(GPUSharedContext).mLightDataBuffer);
    }

    if(mBatchData.mMaterial->getMaterialData().mIsSkinned)
    {
        mShader->bindSharedBuffer(mMeshBatcher.getGPUBuffersLayout().getSharedBuffer(GPUBuiltIn::SharedBuffers::mBonesMatrices));
    }
    
    mShader->bindSharedBuffer(mMeshBatcher.getGPUBuffersLayout().getSharedBuffer(mBatchData.mMaterial->getInstancedPropertiesSharedBufferData()));
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
        GET_SYSTEM(GPUInterface).enableStencil(mBatchData.mStencilData.mStencilValue, mBatchData.mStencilData.mStencilFunction, mBatchData.mStencilData.mStencilPassOp);
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
            //Ptr<Camera> camera = GET_SYSTEM(RenderEngine).mCamera;
            //if(camera && camera->mFrustum.testSphere(renderer->mGameObject->mTransform->getWorldPosition(), renderer->mGameObject->mTransform->getLocalScale().x))
            {
                PROFILER_BLOCK_CPU("update");
                renderer->update();
                addToVertexBuffer(renderer);
            }
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
    mMeshBatcher.addInstance(rendererModelMatrix, renderer->getMeshInstance(), renderer->getMaterialInstance().mMaterialInstancedProperties);
}

bool BatchRenderer::shouldRegenerateBuffers() const
{
    // TODO: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mNewRendererAdded || !mBatchData.mIsStatic || mRegenerateBuffersRequested;
}
