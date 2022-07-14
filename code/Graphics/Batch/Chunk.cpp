#include "Graphics/Batch/Chunk.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Scene/Module.hpp"


Chunk::~Chunk() 
{

	mRenderers.clear();
}

void Chunk::init()
{

	mRenderers.clear();

	mCube.set(Vector3(0, 0, 0), Vector3(0, 0, 0));
}

void Chunk::set(CR(Vector3) leftTop, f32 size)
{
	mCube.set(leftTop, Vector3(size, size, size));
	f32 halfSize = size / 2.0f;
	mCenter.set(leftTop.x + halfSize, leftTop.y - halfSize, leftTop.z - halfSize);

	mRadius = mCenter.dst(leftTop);
}

void Chunk::update()
{
	//RenderEngine::getInstance().drawCube(mCube);

	FOR_LIST(it, mRenderers)
	{
		Ptr<Renderer> renderer = *it;

		bool removeFromList = false;

		if (renderer.get().isActive())
		{
			if (getIsLoaded() && !renderer.get().hasValidBatch())
			{
				RenderEngine::getInstance().assignBatch(renderer);
			}

			if (!renderer.get().isStatic() && !containsRenderer(renderer))
			{
				Ptr<Chunk> newChunk = RenderEngine::getInstance().assignChunk(renderer);

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
			removeFromList = true;
		}

		if (removeFromList)
		{
			it = mRenderers.erase(it);
			--it;
		}
	}
}

void Chunk::load()
{
	if (!mIsLoaded)
	{
		mIsLoaded = true; /*ECHO("load")*/
	}
}

void Chunk::unload()
{
	if (mIsLoaded)
	{
		mIsLoaded = false; /*ECHO("unload")*/
	}
}

void Chunk::addRenderer(Ptr<Renderer> renderer)
{
	mRenderers.push_back(renderer);
}

bool Chunk::containsRenderer(Ptr<const Renderer> renderer, f32 epsilon /*= 0.0f*/) const
{
	Vector3 rendererPosition = renderer.get().getGameObject()->getTransform().get().getWorldPosition();
	bool contains = Geometry::testCubePoint(mCube, rendererPosition, epsilon);
	return contains; // TODO : move to settings ?
}

bool Chunk::containsRendererSphere(Ptr<const Renderer> renderer) const
{
	Vector3 rendererPosition = renderer.get().getGameObject()->getTransform().get().getWorldPosition();
	return Geometry::testSphereSphere(Sphere(mCenter, mRadius), Sphere(rendererPosition, renderer.get().getGameObject()->getTransform().get().getScale().y * 2.0f), 0);
}
