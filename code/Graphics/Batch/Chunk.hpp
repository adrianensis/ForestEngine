#pragma once

#include "Core/Module.hpp"
#include "Graphics/Batch/BatchesMap.hpp"
#include "Graphics/Renderer.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Batch/Chunk.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Batch/BatchesMap.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Scene/Module.hpp"
#endif

class Chunk: public ObjectBase
{
    GENERATE_METADATA(Chunk)

public:
	CPP ~Chunk() override
	{
		// FOR_LIST(it, mRenderers)
		// {
		// 	(*it)->finallyDestroy();
		// 	DELETE((*it));
		// }

		mRenderers.clear();
	}

	CPP void init()
	{
		//TRACE();

		mRenderers.clear();

		mLeftTop.set(0, 0, 0);
	}

	CPP void set(const Vector3& leftTop, f32 size)
	{
		mLeftTop = leftTop;
		mSize = size;
		f32 halfSize = mSize / 2.0f;
		mCenter.set(mLeftTop.x + halfSize, mLeftTop.y - halfSize, 0);

		mRadius = mCenter.dst(mLeftTop);
	}

	CPP void update(BatchesMap *batchesMap)
	{
		//RenderEngine::getInstance().drawRectangle(Rectangle(mLeftTop, mSize, mSize));

		FOR_LIST(it, mRenderers)
		{
			Ptr<Renderer> renderer = *it;

			bool removeFromList = false;

			if (renderer.get().isActive())
			{
				if (getIsLoaded() && !renderer.get().getBatch())
				{
					batchesMap->addRenderer(renderer);
				}

				if (!renderer.get().isStatic() && !containsRenderer(renderer))
				{
					Ptr<Chunk> newChunk = RenderEngine::getInstance().assignChunk(renderer);

					// Only remove the renderer from this chunk if another chunk is found.
					// If not, keep the renderer here until a new chunk is found.
					if (newChunk && newChunk != getRefToThis())
					{
						newChunk.get().addRenderer(renderer);
						removeFromList = true;
					}
				}
			}

			if (renderer.get().getIsPendingToBeDestroyed())
			{
				renderer.get().finallyDestroy();
			}

			if (renderer.get().getIsDestroyed())
			{
				//DELETE(renderer);
				removeFromList = true;
			}

			if (removeFromList)
			{
				it = mRenderers.erase(it);
				--it;
			}
		}
	}

	CPP void load()
	{
		if (!mIsLoaded)
		{
			mIsLoaded = true; /*ECHO("load")*/
		}
	}
	CPP void unload()
	{
		if (mIsLoaded)
		{
			mIsLoaded = false; /*ECHO("unload")*/
		}
	}

	CPP void addRenderer(Ptr<Renderer> renderer)
	{
		mRenderers.push_back(renderer);
	}

	CPP bool containsRenderer(Ptr<const Renderer> renderer, f32 epsilon = 0.0f) const
	{
		Vector3 rendererPosition = renderer.get().getGameObject()->getTransform().get().getWorldPosition();
		bool contains = Geometry::testRectanglePoint(mLeftTop, mSize, mSize, rendererPosition, epsilon);
		return contains; // TODO : move to settings ?
	}

	CPP bool containsRendererSphere(Ptr<const Renderer> renderer) const
	{
		Vector3 rendererPosition = renderer.get().getGameObject()->getTransform().get().getWorldPosition();
		return Geometry::testSphereSphere(mCenter, rendererPosition, mRadius,
										renderer.get().getGameObject()->getTransform().get().getScale().y * 2.0f, 0);
	}

private:
	std::list<Ptr<Renderer>> mRenderers;
	Vector3 mLeftTop;
	f32 mSize = 0.0f;
	bool mIsLoaded = false;
	Vector3 mCenter;
	f32 mRadius = 0.0f;
public:
	GET(IsLoaded)
	GET(Center)
	GET(Radius)
};