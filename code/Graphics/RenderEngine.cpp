#include "Graphics/RenderEngine.hpp"
#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Scene/Module.hpp"
#include "Core/EngineConfig.hpp"
#include "Graphics/Batch/Chunk.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"

void RenderEngine::init(f32 sceneSize)
{
	TRACE()

	REGISTER_COMPONENT_CLASS_IN_ENGINE_SYSTEM(Renderer)

	mCameraDirtyTranslation = true;

	mShapeBatchRendererMapScreenSpace.mIsWorldSpace = false;


	mMinChunkDrawDistance = GET_SYSTEM(EngineConfig).getConfig().at("scene").at("chunks").at("minChunkDrawDistance").get<f32>();

	f32 chunksGridSize = GET_SYSTEM(EngineConfig).getConfig().at("scene").at("chunks").at("gridSize").get<f32>();
	f32 chunksGridSizeHalf = chunksGridSize / 2.0f; // TODO : Make it power of 2!

	mChunks.reserve(chunksGridSize * chunksGridSize);

	f32 chunkSize = sceneSize / ((f32)chunksGridSize);

	for (i32 i = -chunksGridSizeHalf; i < chunksGridSizeHalf; ++i)
	{
		for (i32 j = chunksGridSizeHalf; j > -chunksGridSizeHalf; --j)
		{
			Ptr<Chunk> chunk = mChunks.emplace_back(OwnerPtr<Chunk>::newObject());
			chunk->init();
			chunk->set(Vector3(i * chunkSize, j * chunkSize, chunkSize/2.0f), chunkSize);
		}
	}
}

bool RenderEngine::frustumTestSphere(const Vector3& center, f32 radius)
{
	return mCamera and mCamera->mFrustum.testSphere(center, radius);
}

void RenderEngine::update()
{
	PROFILER_CPU()

	if (mCamera)
	{
		mCamera->update();
	}

	GET_SYSTEM(AnimationManager).update();

	GET_SYSTEM(RenderContext).clear();
	renderBatches();
	swap();
	checkChunks();

}

void RenderEngine::terminate()
{
	TRACE()

	mShapeBatchRendererMap.terminate();
	mShapeBatchRendererMapScreenSpace.terminate();

	mChunks.clear();
}

void RenderEngine::addComponent(Ptr<EngineSystemComponent> component)
{
	EngineSystem::addComponent(component);

	if(component->getClassId() == Renderer::getClassIdStatic())
	{
		Ptr<Renderer> renderer = Ptr<Renderer>::cast(component);

		if (renderer->getIsWorldSpace())
		{
			Ptr<Chunk> chunk = assignChunk(renderer);
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
			mBatchesMap.addRenderer(renderer);
		}
	}
}

Ptr<Chunk> RenderEngine::assignChunk(Ptr<Renderer> renderer)
{
	bool found = false;
	Ptr<Chunk> chunkTmp;
	Ptr<Chunk> chunkFound;
	for (i32 i = 0; (i < (i32)(mChunks.size())) and (!found); ++i)
	{
		chunkTmp = mChunks.at(i);
		if (chunkTmp->containsRenderer /*Sphere*/ (renderer))
		{
			renderer->mChunk = chunkTmp;

			found = true;
			chunkFound = chunkTmp;
		}
	}

	return chunkFound;
}

void RenderEngine::assignBatch(Ptr<Renderer> renderer)
{
	mBatchesMap.addRenderer(renderer);
}

void RenderEngine::drawLine(const Line& line, f32 thickness /*= 1*/, bool isWorldSpace /*= true*/, Vector4 color /*= Vector4(1,1,1,1)*/)
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

void RenderEngine::drawRectangle(const Rectangle& rectangle, f32 thickness/*= 1*/, bool isWorldSpace /*= true*/, Vector4 color /*= Vector4(1,1,1,1)*/)
{
	Vector3 leftTopFront = rectangle.getLeftTopFront();
	Vector3 size = rectangle.getSize();
	drawLine(Line(Vector3(leftTopFront.x, leftTopFront.y, leftTopFront.z), Vector3(leftTopFront.x, leftTopFront.y - size.y, leftTopFront.z)), thickness, isWorldSpace, color);
	drawLine(Line(Vector3(leftTopFront.x, leftTopFront.y - size.y, leftTopFront.z), Vector3(leftTopFront.x + size.x, leftTopFront.y - size.y, leftTopFront.z)), thickness, isWorldSpace, color);
	drawLine(Line(Vector3(leftTopFront.x + size.x, leftTopFront.y - size.y, leftTopFront.z), Vector3(leftTopFront.x + size.x, leftTopFront.y, leftTopFront.z)), thickness, isWorldSpace, color);
	drawLine(Line(Vector3(leftTopFront.x + size.x, leftTopFront.y, leftTopFront.z), Vector3(leftTopFront.x, leftTopFront.y, leftTopFront.z)), thickness, isWorldSpace, color);
}

void RenderEngine::drawCube(const Cube& cube, f32 thickness/*= 1*/, bool isWorldSpace /*= true*/, Vector4 color /*= Vector4(1,1,1,1)*/)
{
	Vector3 leftTopFront = cube.getLeftTopFront();
	Vector3 size = cube.getSize();
	Vector3 leftTopBack = leftTopFront - Vector3(0,0,size.z);
	drawRectangle(Rectangle(leftTopFront, size), thickness, isWorldSpace, color);
	drawRectangle(Rectangle(leftTopBack, size), thickness, isWorldSpace, color);
	drawLine(Line(leftTopFront + Vector3(0,0,0), leftTopBack + Vector3(0,0,0)), thickness, isWorldSpace, color);
	drawLine(Line(leftTopFront + Vector3(size.x,0,0), leftTopBack + Vector3(size.x,0,0)), thickness, isWorldSpace, color);
	drawLine(Line(leftTopFront + Vector3(size.x,-size.y,0), leftTopBack + Vector3(size.x,-size.y,0)), thickness, isWorldSpace, color);
	drawLine(Line(leftTopFront + Vector3(0,-size.y,0), leftTopBack + Vector3(0,-size.y,0)), thickness, isWorldSpace, color);
}

void RenderEngine::swap()
{
	PROFILER_CPU()

	GET_SYSTEM(RenderContext).swap();
}

void RenderEngine::renderBatches()
{
	PROFILER_CPU()

	mBatchesMap.renderStencil();

	mBatchesMap.render();

	mShapeBatchRendererMap.render();
	mShapeBatchRendererMapScreenSpace.render();
}

void RenderEngine::checkChunks()
{
	PROFILER_CPU()

	FOR_ARRAY(i, mChunks)
	{
		Ptr<Chunk> chunk = mChunks.at(i);

		f32 chunkToCameraDistance = chunk->mCenter.dst(mCamera->mGameObject->mTransform->getWorldPosition());
		bool chunkInDistance = chunkToCameraDistance <= mMinChunkDrawDistance;
		
		if (chunkInDistance and !chunk->mIsLoaded)
		{
			chunk->load();
		}
		else if (!chunkInDistance and chunk->mIsLoaded)
		{
			mBatchesMap.forceRegenerateBuffers();

			chunk->unload();
		}

		chunk->update();
	}

}
