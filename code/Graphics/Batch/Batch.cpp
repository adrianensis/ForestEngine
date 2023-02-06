#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Scene/Module.hpp"

void Batch::init(const BatchData& batchData)
{
	mBatchData = batchData;
	mMeshBatcher.init(mBatchData.mMesh, mBatchData.mIsStatic, mBatchData.mIsInstanced);
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
    mBatchData.mMaterial.get().enable();
    mBatchData.mMaterial.get().bind(mBatchData.mIsWorldSpace, mBatchData.mIsInstanced);

    bool isAnimated = mMeshBatcher.isAnimated();
    mBatchData.mMaterial.get().mShader.get().addBool(isAnimated, "hasAnimations");
    
    if(isAnimated)
    {
        const std::vector<Matrix4> & transforms = AnimationManager::getInstance().getBoneTransforms(mMeshBatcher.mPrototypeMesh.get().mModel);
        mBatchData.mMaterial.get().mShader.get().addMatrixArray(transforms, "gBones");
    }

    if(mBatchData.mStencilValue > 0x00)
    {
        RenderContext::enableStencil(mBatchData.mIsStencilMask, mBatchData.mStencilValue, mBatchData.mStencilFunction);
    }
}

void Batch::disable()
{
    RenderContext::disableStencil();
    mBatchData.mMaterial.get().disable();
    mMeshBatcher.disable();
}

void Batch::addRenderer(Ptr<Renderer> renderer)
{
	mRenderers.push_back(renderer);
	renderer.get().mBatch = (getPtrToThis());

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
			/*Transform* transform = renderer->mGameObject.get().mTransform;
			const Vector3& position = transform->getWorldPosition();
			f32 distanceToCamera = position.dst(RenderEngine::getInstance().getCamera()->mGameObject.get().mTransform.get().getWorldPosition());
			if(!renderer->getIsWorldSpace() || distanceToCamera <= renderer->getRenderDistance())*/
			
			addToVertexBuffer(renderer);
		}
	}

    mNewRendererAdded = false;
    mForceRegenerateBuffers = false;
}

bool Batch::shouldRemoveRenderer(Ptr<Renderer> renderer)
{
	bool toRemove = false;

	if(renderer.isValid())
	{
		if (renderer.get().isActive())
		{
			//renderer.get().getChunk
			if (!renderer.get().hasValidChunk())
			{
				toRemove = true;
			}
		}
		else
		{
			if (renderer.get().getIsPendingToBeDestroyed())
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
		renderer.get().mBatch.invalidate();

		if (!mBatchData.mIsWorldSpace)
		{
			renderer.get().finallyDestroy();
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

	if(mBatchData.mIsInstanced)
	{
		renderer.get().update(false);
		const Matrix4& rendererModelMatrix = renderer.get().mRendererModelMatrix;
		mMeshBatcher.addInstanceMatrix(rendererModelMatrix);
	}
	else
	{
		renderer.get().update(true);
		mMeshBatcher.addInstance(renderer.get().generateMeshInstance());
	}
}

bool Batch::shouldRegenerateBuffers() const
{
	return mNewRendererAdded || !mBatchData.mIsStatic || mForceRegenerateBuffers || mMeshBatcher.isAnimated();
}