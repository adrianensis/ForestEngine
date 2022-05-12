#include "Graphics/Batch/Chunk.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer.hpp"
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

	mLeftTop.set(0, 0, 0);
}

void Chunk::set(const Vector3& leftTop, f32 size)
{
	mLeftTop = leftTop;
	mSize = size;
	f32 halfSize = mSize / 2.0f;
	mCenter.set(mLeftTop.x + halfSize, mLeftTop.y - halfSize, mLeftTop.z - halfSize);

	mRadius = mCenter.dst(mLeftTop);
}

void Chunk::update()
{

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
	bool contains = Geometry::testCubePoint(Cube(mLeftTop, Vector3(mSize, mSize, mSize)), rendererPosition, epsilon);
	return contains; // TODO : move to settings ?
}

bool Chunk::containsRendererSphere(Ptr<const Renderer> renderer) const
{
	Vector3 rendererPosition = renderer.get().getGameObject()->getTransform().get().getWorldPosition();
	return Geometry::testSphereSphere(Sphere(mCenter, mRadius), Sphere(rendererPosition, renderer.get().getGameObject()->getTransform().get().getScale().y * 2.0f), 0);
}
