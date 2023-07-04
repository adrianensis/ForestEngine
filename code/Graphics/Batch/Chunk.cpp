#include "Graphics/Batch/Chunk.hpp"

#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
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

void Chunk::set(const Vector3& leftTop, f32 size)
{
	mCube.set(leftTop, Vector3(size, size, size));
	f32 halfSize = size / 2.0f;
	mCenter.set(leftTop.x + halfSize, leftTop.y - halfSize, leftTop.z - halfSize);

	mRadius = mCenter.dst(leftTop);
}

void Chunk::update()
{
	GET_SYSTEM(RenderEngine).drawCube(mCube);

	FOR_LIST(it, mRenderers)
	{
		Ptr<MeshRenderer> renderer = *it;

		bool removeFromList = false;

        if(renderer.isValid())
        {
            if (renderer->isActive())
            {
                if (mIsLoaded and !renderer->getBatch().isValid())
                {
                    GET_SYSTEM(RenderEngine).assignBatch(renderer);
                }

                if (!renderer->isStatic() and !containsRenderer(renderer))
                {
                    Ptr<Chunk> newChunk = GET_SYSTEM(RenderEngine).assignChunk(renderer);

                    if (newChunk and newChunk != getPtrToThis())
                    {
                        newChunk->addRenderer(renderer);
                        removeFromList = true;
                    }
                }
            }

            if (renderer->getIsPendingToBeDestroyed())
            {
                renderer->finallyDestroy();
            }

            if (renderer->getIsDestroyed())
            {
                removeFromList = true;
            }
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

void Chunk::addRenderer(Ptr<MeshRenderer> renderer)
{
	mRenderers.push_back(renderer);
}

bool Chunk::containsRenderer(Ptr<const MeshRenderer> renderer, f32 epsilon /*= 0.0f*/) const
{
	Vector3 rendererPosition = renderer->mGameObject->mTransform->getWorldPosition();
	bool contains = Geometry::testCubePoint(mCube, rendererPosition, epsilon);
	return contains; // TODO : move to settings ?
}

bool Chunk::containsRendererSphere(Ptr<const MeshRenderer> renderer) const
{
	Vector3 rendererPosition = renderer->mGameObject->mTransform->getWorldPosition();
	return Geometry::testSphereSphere(Sphere(mCenter, mRadius), Sphere(rendererPosition, renderer->mGameObject->mTransform->getScale().y * 2.0f), 0);
}
