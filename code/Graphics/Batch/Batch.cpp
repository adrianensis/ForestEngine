#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Shaders/ShaderBuilder.hpp"

void Batch::init(const BatchData& batchData)
{
	mBatchData = batchData;
	mMeshBatcher.init(mBatchData.mMesh, mBatchData.mIsStatic, mBatchData.mIsInstanced);

    const GPUBuffersLayout& gpuBuffersLayout = mMeshBatcher.getGPUBuffersLayout();
    
    mShader = OwnerPtr<Shader>::newObject();
    mShader->init(gpuBuffersLayout, mBatchData.mMaterial);
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
    mBatchData.mMaterial->bind(mShader, mBatchData.mIsWorldSpace, mBatchData.mIsInstanced, mMeshBatcher.isAnimated(), mMeshBatcher.mPrototypeMesh->mModel);

    if(mBatchData.mStencilData.mStencilValue > 0x00)
    {
        GET_SYSTEM(RenderContext).enableStencil(mBatchData.mStencilData.mIsStencilMask, mBatchData.mStencilData.mStencilValue, mBatchData.mStencilData.mStencilFunction);
    }
}

void Batch::disable()
{
    GET_SYSTEM(RenderContext).disableStencil();
    mShader->disable();
    mMeshBatcher.disable();
}

void Batch::addRenderer(Ptr<Renderer> renderer)
{
	mRenderers.push_back(renderer);
	renderer->mBatch = (getPtrToThis());

	mNewRendererAdded = true;
}

void Batch::processRenderers()
{
	PROFILER_CPU()

	u32 newSize = mRenderers.size();
	mMeshBatcher.resize(newSize);
	
	FOR_LIST(it, mRenderers)
	{
		Ptr<Renderer> renderer = *it;

		if (shouldRemoveRenderer(renderer))
		{
			internalRemoveRenderer(it);
		}
		else
		{
			/*Transform* transform = renderer->mGameObject->mTransform;
			const Vector3& position = transform->getWorldPosition();
			f32 distanceToCamera = position.dst(GET_SYSTEM(RenderEngine).getCamera()->mGameObject->mTransform->getWorldPosition());
			if(!renderer->getIsWorldSpace() || distanceToCamera <= renderer->getRenderDistance())*/
			
			addToVertexBuffer(renderer);
		}
	}

    mNewRendererAdded = false;
    mForceRegenerateBuffers = false;
}

bool Batch::shouldRemoveRenderer(Ptr<const Renderer> renderer)
{
	bool toRemove = false;

	if(renderer.isValid())
	{
		if (renderer->isActive())
		{
			//renderer->getChunk
			if (!renderer->hasValidChunk())
			{
				toRemove = true;
			}
		}
		else
		{
			if (renderer->getIsPendingToBeDestroyed())
			{
				toRemove = true;
			}
		}
	}
	else
	{
		toRemove = true;
	}

	return toRemove;
}

void Batch::internalRemoveRenderer(std::list<Ptr<Renderer>>::iterator& it)
{
	PROFILER_CPU()

	Ptr<Renderer> renderer = *it;
	if(renderer)
	{
		renderer->mBatch.invalidate();

		if (!mBatchData.mIsWorldSpace)
		{
			renderer->finallyDestroy();
		}
	}

    it->invalidate();

    mForceRegenerateBuffers = true;

	it = mRenderers.erase(it);
	--it; // go back to the previous it, so the FOR LOOP can do ++it with no problem
}

void Batch::addToVertexBuffer(Ptr<Renderer> renderer)
{
	PROFILER_CPU()

    renderer->update();

    const Matrix4& rendererModelMatrix = renderer->mRendererModelMatrix;
    mMeshBatcher.addInstance(rendererModelMatrix, renderer->getMeshInstance());
}

bool Batch::shouldRegenerateBuffers() const
{
	return mNewRendererAdded || !mBatchData.mIsStatic || mForceRegenerateBuffers || mMeshBatcher.isAnimated();
}