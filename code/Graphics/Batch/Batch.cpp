#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Shaders/ShaderBuilder.hpp"

void Batch::init(const BatchData& batchData)
{
	mBatchData = batchData;
	mMeshBatcher.init(mBatchData);

    const GPUVertexBuffersLayout& gpuVertexBuffersLayout = mMeshBatcher.getGPUVertexBuffersLayout();
    
    mShader = OwnerPtr<Shader>::newObject();
    mShader->init(gpuVertexBuffersLayout, mBatchData.mMaterial);
}

void Batch::render()
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

void Batch::enable()
{
    mMeshBatcher.enable();
    mShader->enable();
    mBatchData.mMaterial->bind(mShader, mBatchData.mIsWorldSpace, mBatchData.mIsInstanced, mBatchData.mMesh);

	Ptr<Camera> camera = GET_SYSTEM(RenderEngine).mCamera;

	const Matrix4& projectionMatrix = camera->mProjectionMatrix;
	const Matrix4& viewMatrix = camera->mViewMatrix;

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);

    GPUSharedContextMatricesData gpuMatricesData = {mBatchData.mIsWorldSpace ? projectionMatrix : ortho, mBatchData.mIsWorldSpace ? viewMatrix : Matrix4::smIdentity};
	GET_SYSTEM(GPUSharedContext).mMatricesBlock.setData(gpuMatricesData);

    if(mBatchData.mStencilData.mUseStencil)
    {
        GET_SYSTEM(GPUInterface).enableStencil(mBatchData.mStencilData.mMaskStencilValue, mBatchData.mStencilData.mStencilFunction, mBatchData.mStencilData.mStencilPassOp);
    }
}

void Batch::disable()
{
    if(mBatchData.mStencilData.mUseStencil)
    {
        GET_SYSTEM(GPUInterface).disableStencil();
    }

    mShader->disable();
    mMeshBatcher.disable();
}

void Batch::addRenderer(Ptr<MeshRenderer> renderer)
{
	mRenderers.push_back(renderer);
	renderer->setBatch(getPtrToThis());

	mNewRendererAdded = true;
}

void Batch::processRenderers()
{
	PROFILER_CPU()

	u32 newSize = mRenderers.size();
	mMeshBatcher.resize(newSize);
	
	FOR_LIST(it, mRenderers)
	{
		Ptr<MeshRenderer> renderer = *it;

		if (shouldRemoveRenderer(renderer))
		{
			internalRemoveRenderer(it);
		}
		else
		{
			renderer->preUpdate();
		}
	}

    FOR_LIST(it, mRenderers)
	{
		Ptr<MeshRenderer> renderer = *it;
        renderer->update();
		addToVertexBuffer(renderer);
	}

    mNewRendererAdded = false;
    mRegenerateBuffersRequested = false;
}

bool Batch::shouldRemoveRenderer(Ptr<const MeshRenderer> renderer)
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

void Batch::internalRemoveRenderer(std::list<Ptr<MeshRenderer>>::iterator& it)
{
	PROFILER_CPU()

	Ptr<MeshRenderer> renderer = *it;
	if(renderer)
	{
		renderer->setBatch(Ptr<Batch>());

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

void Batch::addToVertexBuffer(Ptr<MeshRenderer> renderer)
{
	PROFILER_CPU()

    const Matrix4& rendererModelMatrix = renderer->getRendererModelMatrix();
    mMeshBatcher.addInstance(rendererModelMatrix, renderer->getMeshInstance());
}

bool Batch::shouldRegenerateBuffers() const
{
    // TODO: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mNewRendererAdded || !mBatchData.mIsStatic || mRegenerateBuffersRequested;
}
