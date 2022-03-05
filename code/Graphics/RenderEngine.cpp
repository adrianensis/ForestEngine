#include "Graphics/RenderEngine.hpp"
#include "Graphics/Optimizations/Batch.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Scene/Module.hpp"
#include "Core/EngineConfig.hpp"
#include "Graphics/Optimizations/Chunk.hpp"
#include "Graphics/ShapeBatchRenderer.hpp"

void RenderEngine::init(f32 sceneSize)
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
			Chunk *chunk = NEW(Chunk);
			chunk->init();
			chunk->set(Vector2(i * chunkSize, j * chunkSize), chunkSize);

			mChunks.push_back(chunk);
		}
	}

	mBatchesMap.init();
}

bool RenderEngine::frustumTestSphere(const Vector3& center, f32 radius)
{
	return mCamera && mCamera->getFrustum().testSphere(center, radius);
}

void RenderEngine::update()
{
	PROFILER_TIMEMARK_START()

	if (mCamera)
	{
		mCamera->update();
	}

	renderBatches();
	swap();
	checkChunks();

	PROFILER_TIMEMARK_END()
}

void RenderEngine::swap()
{
	PROFILER_TIMEMARK_START()

	RenderContext::swap();

	PROFILER_TIMEMARK_END()
}

void RenderEngine::renderBatches()
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

void RenderEngine::checkChunks()
{
	PROFILER_TIMEMARK_START()

	FOR_ARRAY(i, mChunks)
	{
		Chunk *chunk = mChunks.at(i);

		f32 chunkToCameraDistance = chunk->getCenter().dst(mCamera->getGameObject()->getTransform()->getWorldPosition());
		bool chunkInDistance = chunkToCameraDistance <= mMinChunkDrawDistance;
		
		if (chunkInDistance && !chunk->getIsLoaded())
		{
			chunk->load();
		}
		else if (!chunkInDistance && chunk->getIsLoaded())
		{
			mBatchesMap.forceRegenerateBuffers();

			chunk->unload();
		}

		//if (chunk->getIsLoaded()) {
		chunk->update(&mBatchesMap);
		//}
	}

	PROFILER_TIMEMARK_END()
}

void RenderEngine::terminate()
{
	TRACE()

	mShapeBatchRendererMap.terminate();
	mShapeBatchRendererMapScreenSpace.terminate();

	DELETE_CONTENT(mChunks);
}

void RenderEngine::addComponent(IEngineSystemComponent *component)
{
	IEngineSystem::addComponent(component);

	if(component->getClassId() == Renderer::getClassIdStatic())
	{
		Renderer *renderer = static_cast<Renderer*>(component);

		/*if(!CONTAINS(mDepthsData, renderer->getDepth()))
		{
			DepthData DepthData;
			DepthData.mDepthNumber = renderer->getDepth();
			MAP_INSERT(mDepthsData, renderer->getDepth(), DepthData);
		}*/

		if (renderer->getIsWorldSpace())
		{
			Chunk *chunk = assignChunk(renderer);
			if (chunk)
			{
				chunk->addRenderer(renderer);
			}
			else
			{
				ASSERT_MSG(false, "Renderer can't find a chunk.")
			}
		}
		else
		{
			// UI Case!
			mBatchesMap.addRenderer(*renderer);
		}
	}
}

Chunk *RenderEngine::assignChunk(Renderer *renderer)
{
	//TRACE();
	bool found = false;
	Chunk *chunkTmp = nullptr;
	Chunk *chunkFound = nullptr;
	// FOR_ARRAY_COND(i, mChunks, !found) {
	for (i32 i = 0; (i < (i32)(mChunks.size())) && (!found); ++i)
	{
		// FOR_ARRAY(i, mChunks){
		chunkTmp = mChunks.at(i);
		if (chunkTmp->containsRenderer /*Sphere*/ (renderer))
		{
			renderer->setChunk(chunkTmp);

			// if(! renderer->isStatic()){
			found = true;
			chunkFound = chunkTmp;
			// }
		}
	}

	return chunkFound;
}

void RenderEngine::drawLine(const Line& line, f32 thickness /*= 1*/, bool isWorldSpace /*= true*/, Vector4 color /* = Vector4(1,1,1,1)*/)
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

void RenderEngine::drawRectangle(const Rectangle& rectangle, f32 thickness, bool isWorldSpace, Vector4 color)
{
	Vector3 leftTop = rectangle.getLeftTop();
	Vector3 size = rectangle.getSize();
	drawLine(Line(Vector3(leftTop.x, leftTop.y, leftTop.z), Vector3(leftTop.x, leftTop.y - size.y, leftTop.z)), thickness, isWorldSpace, color);
	drawLine(Line(Vector3(leftTop.x, leftTop.y - size.y, leftTop.z), Vector3(leftTop.x + size.x, leftTop.y - size.y, leftTop.z)), thickness, isWorldSpace, color);
	drawLine(Line(Vector3(leftTop.x + size.x, leftTop.y - size.y, leftTop.z), Vector3(leftTop.x + size.x, leftTop.y, leftTop.z)), thickness, isWorldSpace, color);
	drawLine(Line(Vector3(leftTop.x + size.x, leftTop.y, leftTop.z), Vector3(leftTop.x, leftTop.y, leftTop.z)), thickness, isWorldSpace, color);
}

// void RenderEngine::drawRectangle(const Rectangle& rectangle, f32 thickness, bool isWorldSpace, Vector4 color)
// {
// 	Vector3 leftTopFront = rectangle.getLeftTop();
// 	Vector3 leftTopBack = rectangle.getLeftTop() + Vector3(0,0,rectangle.getSize().z);
// 	Vector3 size = rectangle.getSize();
	
// 	drawRectangle(Rectangle(leftTopFront, size), thickness, isWorldSpace, color);
// 	drawRectangle(Rectangle(leftTopBack, size), thickness, isWorldSpace, color);
// 	// TODO : draw 3D rectangle
// }