#include "Graphics/Optimizations/Batch.hpp"
#include "Core/Time/TimeUtils.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/RenderContext.hpp"
#include "Log/Log.hpp"
#include "Scene/GameObject.hpp"
#include "Scene/Transform.hpp"
#include "Maths/MathUtils.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Optimizations/Chunk.hpp"
#include "Profiler/Profiler.hpp"

Batch::~Batch()
{
	// TODO : Is this needed if !WorldSpace??

	if (!mIsWorldSpace)
	{
		FOR_LIST(itRenderer, mRenderers)
		{
			if (!(*itRenderer)->getIsDestroyed())
			{
				(*itRenderer)->finallyDestroy();
				DELETE((*itRenderer));
			}
		}

		mRenderers.clear();
	}
	else
	{
		LIST_DELETE_CONTENT(mRenderers)
	}

	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBOPosition);
	glDeleteBuffers(1, &mVBOTexture);
	glDeleteBuffers(1, &mVBOColor);
	//glDeleteBuffers(1, &mVBONormal);
	glDeleteBuffers(1, &mEBO);
}

void Batch::init(const Mesh *mesh, Material *material)
{
	// TRACE();

	mMesh = mesh;
	mMaterial = material;
	mIsWorldSpace = true;

	mMaxMeshesIncrement = 100;
	mMeshesIndex = 0;

	bind();
}

void Batch::bind()
{
	mVAO = RenderContext::createVAO();
	mVBOPosition = RenderContext::createVBO(Mesh::smVertexPositionSize, 0);
	mVBOTexture = RenderContext::createVBO(Mesh::smVertexTexCoordSize, 1);
	mVBOColor = RenderContext::createVBO(Mesh::smVertexColorSize, 2);
	//mVBONormal = RenderContext::createVBO(mMesh->getNormals(), 3, 3);
	mEBO = RenderContext::createEBO();

	Texture *texture = mMaterial->getTexture();

	if (texture)
	{
		texture->bind();
	}

	RenderContext::enableVAO(0);
}

void Batch::render()
{
	PROFILER_TIMEMARK_START()

	if (!mRenderers.empty())
	{
		RenderContext::enableVAO(mVAO);

		RenderContext::enableProperty(0);
		RenderContext::enableProperty(1);
		RenderContext::enableProperty(2);

		mMaterial->enable();

		mMaterial->bind(mIsWorldSpace);

		if(shouldRegenerateBuffers())
		{
			resizeVertexBuffers(mRenderers.size());
			mPendingDrawCall = processRenderers(mRenderers);
		}
		else
		{
			mPendingDrawCall = true;
		}

		if(mPendingDrawCall)
		{
			drawCall(); // flush all the previous rendereres
		}

		mMaterial->disable();

		RenderContext::disableProperty(0);
		RenderContext::disableProperty(1);
		RenderContext::disableProperty(2);

		RenderContext::enableVAO(0);
	}

	PROFILER_TIMEMARK_END()
}

void Batch::resizeVertexBuffers(u32 newSize)
{
	PROFILER_TIMEMARK_START()
	if (newSize > mMaxMeshesThreshold)
	{
		mMaxMeshesThreshold += mMaxMeshesIncrement;

		mMeshBuilder.init(mMesh->getVertexCount() * mMaxMeshesThreshold, mMesh->getFacesCount() * mMaxMeshesThreshold);

		RenderContext::resizeVBO(mVBOPosition, mMeshBuilder.getVertices().capacity());
		RenderContext::resizeVBO(mVBOTexture, mMeshBuilder.getTextureCoordinates().capacity());
		RenderContext::resizeVBO(mVBOColor, mMeshBuilder.getColors().capacity());

		// Create Faces once and send to GPU once.
		FOR_RANGE(i, 0, mMaxMeshesThreshold)
		{
			i32 offset = +(4 * i);
			mMeshBuilder.addFace(0 + offset, 1 + offset, 3 + offset)->addFace(1 + offset, 2 + offset, 3 + offset);
		}

		// NOTE : VAO needs to be enabled before this line
		RenderContext::resizeEBO(mEBO, mMeshBuilder.getFaces().size());
		RenderContext::setDataEBO(mEBO, mMeshBuilder.getFaces());
	}
	else
	{
		mMeshBuilder.clear();
	}

	mMeshesIndex = 0;
	PROFILER_TIMEMARK_END()
}

bool Batch::processRenderers(std::list<Renderer *>& renderers)
{
	PROFILER_TIMEMARK_START()

	u32 renderersSize = mRenderers.size();

	bool pendingDrawCall = false;
	
	FOR_LIST(it, mRenderers)
	{
		Renderer *renderer = *it;

		bool toRemove = false;

		if (renderer->isActive())
		{
			if (isChunkOk(renderer))
			{
				//if (!checkIsOutOfCamera(camera, renderer)) { }

				if(renderer->hasClipRectangle())
				{
					/*if(pendingDrawCall)
					{
						drawCall(); // flush all the previous rendereres
						resizeVertexBuffers(renderers->size()); // TODO : resize to the correct remaining size
					}

					addToVertexBuffer(renderer);

					mMaterial->getShader()->addVector2(renderer->getClipRectangle().getLeftTop(), "clipRegionLeftTop");
					mMaterial->getShader()->addVector2(renderer->getClipRectangle().getSize(), "clipRegionSize");

					drawCall();
					resizeVertexBuffers(renderers->size());

					// TODO : comment this ↓↓↓↓ to test clip rectangle
					mMaterial->getShader()->addVector2(Vector2(), "clipRegionLeftTop");
					mMaterial->getShader()->addVector2(Vector2(), "clipRegionSize");*/
				}
				else
				{
					addToVertexBuffer(renderer);
					pendingDrawCall = true;
				}
			}
			else
			{
				toRemove = true;
			}
		}
		else
		{
			if (renderer->getIsPendingToBeDestroyed())
			{
				toRemove = true;
			}
		}

		if (toRemove)
		{
			internalRemoveRendererFromList(it, mRenderers);
		}
	}

	PROFILER_TIMEMARK_END()

	return pendingDrawCall;
}

bool Batch::isChunkOk(Renderer *renderer) const
{
	const Chunk *chunk = renderer->getChunk();
	return (!chunk) || (chunk && chunk->getIsLoaded()); // !chunk means -> Screen Space case
}

void Batch::drawCall()
{
	PROFILER_TIMEMARK_START()
	if (mMeshesIndex > 0)
	{
		RenderContext::setDataVBO(mVBOPosition, mMeshBuilder.getVertices());
		RenderContext::setDataVBO(mVBOTexture, mMeshBuilder.getTextureCoordinates());
		RenderContext::setDataVBO(mVBOColor, mMeshBuilder.getColors());

		RenderContext::drawRectangles(mMeshesIndex);
	}

	mNewRendererAdded = false;
	mPendingDrawCall = false;

	PROFILER_TIMEMARK_END()
}

void Batch::insertSorted(Renderer *renderer, std::list<Renderer *> *renderers)
{
	// INSERT SORTED

	f32 y = renderer->getGameObject()->getTransform()->getWorldPosition().y;

	renderers->push_back(renderer);

	// CASE 1 : IF LIST IS EMPTY
	/*if (renderers->empty()) {
		renderers->push_back(renderer);
	} else {
		Renderer* first = *renderers->begin();
		Renderer* last = *renderers->end();

		// CASE 2 : RENDERER IS IN THE LAST/FIRST Depth
		if (last->isActive() && (y <= last->getGameObject()->getTransform()->getWorldPosition().y)) {
			renderers->push_back(renderer);
		} else if (first->isActive() && (y >= first->getGameObject()->getTransform()->getWorldPosition().y)) {
			renderers->push_front(renderer);
		} else {
			// CASE 3 : LIST HAS ELEMENTS AND RENDERER IS IN A RANDOM Depth, NOT THE LAST
			bool foundSmallerY = false;

			// TODO : complete insert sorted
			auto itSmallerY = renderers->getIterator();

			FOR_LIST_COND(it, renderers, !foundSmallerY)
			{
				Renderer* otherRenderer = it.get();
				if(otherRenderer->isActive()){
					f32 otherY = otherRenderer->getGameObject()->getTransform()->getWorldPosition().y;

					if (y >= otherY) {
						foundSmallerY = true;
						itSmallerY = it;
					}
				}
			}

			if (foundSmallerY) {
				renderers->insert(itSmallerY, renderer); // this method inserts before the iterator
			} else {
				renderers->pushBack(renderer);
			}

			renderers->push_back(renderer);
		}
	}*/
}

void Batch::addRenderer(Renderer *renderer)
{
	u32 Depth = renderer->getDepth();

	/*if (RenderEngine::getInstance().getDepthsData().at(renderer->getDepth()).mSorted)
	{
		insertSorted(renderer, renderers);
	}
	else
	{
		// renderers->push_back(renderer);
	}*/

	mRenderers.push_back(renderer);

	renderer->setIsAlreadyInBatch(true);

	mNewRendererAdded = true;
}

void Batch::internalRemoveRendererFromList(std::list<Renderer *>::iterator &it, std::list<Renderer *>& list)
{
	PROFILER_TIMEMARK_START()

	Renderer *renderer = *it;

	renderer->setIsAlreadyInBatch(false);

	// NOTE: UI CASE
	// UI is not deleted in Chunk so it has to be deleted here.
	if (!mIsWorldSpace)
	{
		renderer->finallyDestroy();
		DELETE(renderer);
	}

	it = list.erase(it);
	--it; // go back to the previous it, so the FOR LOOP can do ++it with no problem

	PROFILER_TIMEMARK_END()
}

void Batch::addToVertexBuffer(Renderer *renderer)
{
	PROFILER_TIMEMARK_START()
	renderer->updateAnimation();

	const std::vector<Vector2> &vertexPositions = renderer->getVertices();

	FOR_RANGE(i, 0, mMesh->getVertexCount())
	{
		mMeshBuilder.addVertex(vertexPositions[i]);

		Vector2 vertexTexture(
			mMesh->getTextureCoordinates()[i * Mesh::smVertexTexCoordSize + 0],
			mMesh->getTextureCoordinates()[i * Mesh::smVertexTexCoordSize + 1]);

		Vector2 regionSize = renderer->getRegion().getSize();
		Vector2 regionPosition = renderer->getRegion().getLeftTop();

		Vector2 textureCoord(vertexTexture.x * regionSize.x + regionPosition.x, vertexTexture.y * regionSize.y + regionPosition.y);

		if (renderer->getInvertAxisX())
		{
			textureCoord.x = 1.0f - textureCoord.x;

			const Animation *animation = renderer->getCurrentAnimation();

			if (animation)
			{
				textureCoord.x = textureCoord.x - (1.0f - (animation->getNumberOfFrames() * regionSize.x));
			}
		}

		mMeshBuilder.addTexCoord(textureCoord.x, textureCoord.y);

		mMeshBuilder.addColor(
			renderer->getColor()[0],
			renderer->getColor()[1],
			renderer->getColor()[2],
			renderer->getColor()[3]);
	}

	mMeshesIndex++;
	
	PROFILER_TIMEMARK_END()
}

bool Batch::shouldRegenerateBuffers() const
{
	return mNewRendererAdded || !mIsStatic;
}
