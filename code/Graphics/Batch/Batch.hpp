#pragma once

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Buffers/VertexBuffer.hpp"
#include "Graphics/Renderer.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Batch/Chunk.hpp"
#include "Scene/Module.hpp"
#endif

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

private:
	std::list<Ptr<Renderer>> mRenderers;

	Ptr<Material> mMaterial;
	bool mIsInstanced = false;

	MeshBatcher mMeshBatcher;

	bool mIsWorldSpace = false;
	bool mIsStatic = false;

	bool mNewRendererAdded = false;
	bool mPendingDrawCall = false;
	bool mForceRegenerateBuffers = false;

	CPP bool processRenderers()
	{
		PROFILER_TIMEMARK_START()

		u32 newSize = mRenderers.size();
		mMeshBatcher.resize(newSize);

		bool pendingDrawCall = false;
		
		FOR_LIST(it, mRenderers)
		{
			Ptr<Renderer> renderer = *it;

			bool toRemove = false;

			if(renderer.isValid())
			{
				if (renderer.get().isActive())
				{
					if (!isChunkOk(renderer))
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

			if (toRemove)
			{
				internalRemoveRendererFromList(it);
			}
			else
			{
				/*Transform* transform = renderer->getGameObject()->getTransform();
				const Vector3& position = transform->getWorldPosition();
				f32 distanceToCamera = position.dst(RenderEngine::getInstance().getCamera()->getGameObject()->getTransform().get().getWorldPosition());
				if(!renderer->getIsWorldSpace() || distanceToCamera <= renderer->getRenderDistance())*/
				
				if(renderer.get().hasClipRectangle())
				{
					/*if(pendingDrawCall)
					{
						drawCall(); // flush all the previous rendereres
						resizeBuffers(renderers->size()); // TODO : resize to the correct remaining size
					}

					addToVertexBuffer(renderer);

					mMaterial->getShader()->addVector2(renderer->getClipRectangle().getLeftTop(), "clipTextureRegionLeftTop");
					mMaterial->getShader()->addVector2(renderer->getClipRectangle().getSize(), "clipTextureRegionSize");

					drawCall();
					resizeBuffers(renderers->size());

					// TODO : comment this ↓↓↓↓ to test clip rectangle
					mMaterial->getShader()->addVector2(Vector2(), "clipTextureRegionLeftTop");
					mMaterial->getShader()->addVector2(Vector2(), "clipTextureRegionSize");*/
				}
				else
				{
					addToVertexBuffer(renderer);
					pendingDrawCall = true;
				}
			}
		}

		PROFILER_TIMEMARK_END()

		return pendingDrawCall;
	}

	CPP bool isChunkOk(Ptr<Renderer> renderer) const
	{
		Ptr<Chunk> chunk = renderer.get().getChunk();
		return (!chunk) || (chunk && chunk.get().getIsLoaded()); // !chunk means -> Screen Space case
	}



	CPP void internalRemoveRendererFromList(std::list<Ptr<Renderer>>::iterator& it)
	{
		PROFILER_TIMEMARK_START()

		Ptr<Renderer> renderer = *it;

		if(renderer)
		{
			renderer.get().setBatch(Ptr<Batch>());

			// NOTE: UI CASE
			// UI is not deleted in Chunk so it has to be deleted here.
			if (!getIsWorldSpace())
			{
				renderer.get().finallyDestroy();
				//DELETE(renderer);
			}
		}

		it = mRenderers.erase(it);
		--it; // go back to the previous it, so the FOR LOOP can do ++it with no problem

		PROFILER_TIMEMARK_END()
	}

	CPP void addToVertexBuffer(Ptr<Renderer> renderer)
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

	CPP bool shouldRegenerateBuffers() const
	{
		return mNewRendererAdded || !mIsStatic || mForceRegenerateBuffers;
	}

public:
	~Batch() override
	{
		// FOR_LIST(it, mRenderers)
		// {
		// 	if (it->isValid())
		// 	{
		// 		it->disconnectFromOwner();
		// 		DELETE(it->getObject());
		// 	}
		// }

		mRenderers.clear();
		mMeshBatcher.terminate();
	}

	CPP void init(Ptr<const Mesh> mesh, Ptr<Material> material, bool isStatic, bool isWorldSpace)
	{
		// TRACE();

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

	CPP void render()
	{
		PROFILER_TIMEMARK_START()

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

		PROFILER_TIMEMARK_END()
	}

	CPP void addRenderer(Ptr<Renderer> renderer)
	{
		mRenderers.push_back(renderer);
		renderer.get().setBatch(getRefToThis());

		mNewRendererAdded = true;
	}

	void forceRegenerateBuffers() { mForceRegenerateBuffers = true; }

	RGET(Material)
	GET(IsInstanced)
	GET(IsWorldSpace)
	GET(IsStatic)
};