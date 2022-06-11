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
#include "Scene/Module.hpp"

Batch::~Batch()
{
	mRenderers.clear();
	mMeshBatcher.terminate();
}

void Batch::init(Ptr<const Mesh> mesh, Ptr<Material> material, bool isStatic, bool isWorldSpace)
{
	mMaterial = material;
	mIsWorldSpace = isWorldSpace;
	mIsStatic = isStatic;

	mMeshBatcher.init(mesh, mIsStatic, mIsInstanced);

	Ptr<Texture> texture = mMaterial.get().getTexture();
	if (texture)
	{
		texture.get().bind();
	}
	
	mMeshBatcher.disable();
}

void Batch::render()
{
	PROFILER_CPU()

	if (!mRenderers.empty())
	{
		mMeshBatcher.enable();
		mMaterial.get().enable();
		mMaterial.get().bind(getIsWorldSpace(), getIsInstanced());

		mPendingDrawCall = true;

		if(shouldRegenerateBuffers())
		{
			mPendingDrawCall = processRenderers();
		}

		if(mPendingDrawCall)
		{
			mMeshBatcher.drawCall();

			mNewRendererAdded = false;
			mPendingDrawCall = false;
			mForceRegenerateBuffers = false;
		}

		mMaterial.get().disable();
		mMeshBatcher.disable();
	}
}

void Batch::addRenderer(Ptr<Renderer> renderer)
{
	mRenderers.push_back(renderer);
	renderer.get().setBatch(getRefToThis());

	mNewRendererAdded = true;
}

bool Batch::processRenderers()
{
	PROFILER_CPU()

	u32 newSize = mRenderers.size();
	mMeshBatcher.resize(newSize);

	bool pendingDrawCall = false;
	
	FOR_LIST(it, mRenderers)
	{
		Ptr<Renderer> renderer = *it;

		if (shouldRemoveRenderer(renderer))
		{
			internalRemoveRendererFromList(it);
		}
		else
		{
			/*Transform* transform = renderer->getGameObject()->getTransform();
			const Vector3& position = transform->getWorldPosition();
			f32 distanceToCamera = position.dst(RenderEngine::getInstance().getCamera()->getGameObject()->getTransform().get().getWorldPosition());
			if(!renderer->getIsWorldSpace() || distanceToCamera <= renderer->getRenderDistance())*/
			
			addToVertexBuffer(renderer);
			pendingDrawCall = true;
		}
	}


	return pendingDrawCall;
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

void Batch::internalRemoveRendererFromList(std::list<Ptr<Renderer>>::iterator& it)
{
	PROFILER_CPU()

	Ptr<Renderer> renderer = *it;

	if(renderer)
	{
		renderer.get().setBatch(Ptr<Batch>());

		if (!getIsWorldSpace())
		{
			renderer.get().finallyDestroy();
		}
	}

	it = mRenderers.erase(it);
	--it; // go back to the previous it, so the FOR LOOP can do ++it with no problem

}

void Batch::addToVertexBuffer(Ptr<Renderer> renderer)
{
	renderer.get().update();

	if(getIsInstanced())
	{
		const Matrix4& rendererModelMatrix = renderer.get().getRendererModelMatrix();
		mMeshBatcher.addInstanceMatrix(rendererModelMatrix);
	}
	else
	{
		mMeshBatcher.addInstance(renderer.get().generateMeshInstance());
	}
}

bool Batch::shouldRegenerateBuffers() const
{
	return mNewRendererAdded || !mIsStatic || mForceRegenerateBuffers;
}
