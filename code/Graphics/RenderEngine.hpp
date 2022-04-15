#pragma once

#include "Core/Module.hpp"
#include "Graphics/Batch/BatchesMap.hpp"
#include "Graphics/ShapeBatchRenderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Batch/Chunk.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Scene/Module.hpp"
#include "Core/EngineConfig.hpp"
#include "Graphics/Batch/Chunk.hpp"
#include "Graphics/ShapeBatchRenderer.hpp"
#endif

class RenderEngine : public IEngineSystem, public Singleton<RenderEngine>
{
	GENERATE_METADATA(RenderEngine)

public:
	CPP void init(f32 sceneSize)
	{
		TRACE()

		IEngineSystem::init();

		REGISTER_COMPONENT_CLASS_IN_ENGINE_SYSTEM(Renderer)

		mCameraDirtyTranslation = true;

		mShapeBatchRendererMapScreenSpace.setIsWorldSpace(false);

		// Static Chunks grid

		mMinChunkDrawDistance = EngineConfig::getInstance().getConfig().at("scene").at("chunks").at("minChunkDrawDistance").get<f32>();

		f32 chunksGridSize = EngineConfig::getInstance().getConfig().at("scene").at("chunks").at("gridSize").get<f32>();
		f32 chunksGridSizeHalf = chunksGridSize / 2.0f; // TODO : Make it power of 2!

		mChunks.reserve(chunksGridSize * chunksGridSize);

		f32 chunkSize = sceneSize / ((f32)chunksGridSize);

		for (i32 i = -chunksGridSizeHalf; i < chunksGridSizeHalf; ++i)
		{
			for (i32 j = chunksGridSizeHalf; j > -chunksGridSizeHalf; --j)
			{
				OwnerPtr<Chunk> chunk = OwnerPtr<Chunk>(NEW(Chunk));
				chunk.get().init();
				chunk.get().set(Vector2(i * chunkSize, j * chunkSize), chunkSize);

				mChunks.push_back(chunk);
			}
		}

		mBatchesMap.init();
	}

	CPP bool frustumTestSphere(const Vector3& center, f32 radius)
	{
		return mCamera && mCamera.get().getFrustum().testSphere(center, radius);
	}

	CPP void update()
	{
		PROFILER_TIMEMARK_START()

		if (mCamera)
		{
			mCamera.get().update();
		}

		renderBatches();
		swap();
		checkChunks();

		PROFILER_TIMEMARK_END()
	}

	CPP void terminate()
	{
		TRACE()

		mShapeBatchRendererMap.terminate();
		mShapeBatchRendererMapScreenSpace.terminate();

		//DELETE_CONTENT(mChunks);
		mChunks.clear();
	}

	CPP void addComponent(Ptr<IEngineSystemComponent> component)
	{
		IEngineSystem::addComponent(component);

		if(component.get().getClassId() == Renderer::getClassIdStatic())
		{
			Ptr<Renderer> renderer = Ptr<Renderer>::Cast(component);

			/*if(!CONTAINS(mDepthsData, renderer->getDepth()))
			{
				DepthData DepthData;
				DepthData.mDepthNumber = renderer->getDepth();
				MAP_INSERT(mDepthsData, renderer->getDepth(), DepthData);
			}*/

			if (renderer.get().getIsWorldSpace())
			{
				Ptr<Chunk> chunk = assignChunk(renderer);
				if (chunk)
				{
					chunk.get().addRenderer(renderer);
				}
				else
				{
					ASSERT_MSG(false, "Renderer can't find a chunk.")
				}
			}
			else
			{
				// UI Case!
				mBatchesMap.addRenderer(renderer);
			}
		}
	}

	CPP Ptr<Chunk> assignChunk(Ptr<Renderer> renderer)
	{
		//TRACE();
		bool found = false;
		Ptr<Chunk> chunkTmp;
		Ptr<Chunk> chunkFound;
		// FOR_ARRAY_COND(i, mChunks, !found) {
		for (i32 i = 0; (i < (i32)(mChunks.size())) && (!found); ++i)
		{
			// FOR_ARRAY(i, mChunks){
			chunkTmp = mChunks.at(i);
			if (chunkTmp.get().containsRenderer /*Sphere*/ (renderer))
			{
				renderer.get().setChunk(chunkTmp);

				// if(! renderer->isStatic()){
				found = true;
				chunkFound = chunkTmp;
				// }
			}
		}

		return chunkFound;
	}

	CPP void assignBatch(Ptr<Renderer> renderer)
	{
		mBatchesMap.addRenderer(renderer);
	}

	CPP void drawLine(const Line& line, f32 thickness = 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1))
	{
		if (isWorldSpace)
		{
			mShapeBatchRendererMap.add(line, thickness, color);
		}
		else
		{
			mShapeBatchRendererMapScreenSpace.add(line, thickness, color);
		}
	}

	CPP void drawRectangle(const Rectangle& rectangle, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1))
	{
		Vector3 leftTop = rectangle.getLeftTop();
		Vector3 size = rectangle.getSize();
		drawLine(Line(Vector3(leftTop.x, leftTop.y, leftTop.z), Vector3(leftTop.x, leftTop.y - size.y, leftTop.z)), thickness, isWorldSpace, color);
		drawLine(Line(Vector3(leftTop.x, leftTop.y - size.y, leftTop.z), Vector3(leftTop.x + size.x, leftTop.y - size.y, leftTop.z)), thickness, isWorldSpace, color);
		drawLine(Line(Vector3(leftTop.x + size.x, leftTop.y - size.y, leftTop.z), Vector3(leftTop.x + size.x, leftTop.y, leftTop.z)), thickness, isWorldSpace, color);
		drawLine(Line(Vector3(leftTop.x + size.x, leftTop.y, leftTop.z), Vector3(leftTop.x, leftTop.y, leftTop.z)), thickness, isWorldSpace, color);
	}

private:
	CPP void swap()
	{
		PROFILER_TIMEMARK_START()

		RenderContext::swap();

		PROFILER_TIMEMARK_END()
	}

	CPP void renderBatches()
	{
		PROFILER_TIMEMARK_START()

		/*FOR_MAP(it, mDepthsData)
		{
			if (it->second.mVisible)
			{
				
			}
		}*/

		mBatchesMap.render();

		mShapeBatchRendererMap.render();
		mShapeBatchRendererMapScreenSpace.render();
		
		PROFILER_TIMEMARK_END()
	}

	CPP void checkChunks()
	{
		PROFILER_TIMEMARK_START()

		FOR_ARRAY(i, mChunks)
		{
			Ptr<Chunk> chunk = mChunks.at(i);

			f32 chunkToCameraDistance = chunk.get().getCenter().dst(mCamera.get().getGameObject()->getTransform().get().getWorldPosition());
			bool chunkInDistance = chunkToCameraDistance <= mMinChunkDrawDistance;
			
			if (chunkInDistance && !chunk.get().getIsLoaded())
			{
				chunk.get().load();
			}
			else if (!chunkInDistance && chunk.get().getIsLoaded())
			{
				mBatchesMap.forceRegenerateBuffers();

				chunk.get().unload();
			}

			//if (chunk->getIsLoaded()) {
			chunk.get().update();
			//}
		}

		PROFILER_TIMEMARK_END()
	}
private:
	BatchesMap mBatchesMap;

	ShapeBatchRendererMap mShapeBatchRendererMap;
	ShapeBatchRendererMap mShapeBatchRendererMapScreenSpace;

	Ptr<Camera> mCamera;
	bool mCameraDirtyTranslation = false;

	f32 mMinChunkDrawDistance = 0.0f;
	std::vector<OwnerPtr<Chunk>> mChunks;

public:
	RGET_SET(Camera)
	GET(CameraDirtyTranslation)
	GET(MinChunkDrawDistance)
};