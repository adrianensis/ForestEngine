#include "Graphics/RenderEngine.hpp"
#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Scene/Module.hpp"
#include "Engine/EngineConfig.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"

void RenderEngine::init(f32 sceneSize)
{
	TRACE()

	REGISTER_COMPONENT_CLASS_IN_ENGINE_SYSTEM(MeshRenderer)

	mCameraDirtyTranslation = true;

	mShapeBatchRenderer.init(true, 2);
	mShapeBatchRendererScreenSpace.init(false, 2);

	// octree.init(5000);
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

	// octree.update();

	GET_SYSTEM(RenderContext).clear();
	renderBatches();
	swap();
}

void RenderEngine::terminate()
{
	TRACE()

	mShapeBatchRenderer.terminate();
	mShapeBatchRendererScreenSpace.terminate();
}

void RenderEngine::addComponent(Ptr<EngineSystemComponent> component)
{
	EngineSystem::addComponent(component);

    Ptr<MeshRenderer> renderer = Ptr<MeshRenderer>::cast(component);
    ASSERT_MSG(renderer.isValid(), "Trying to add a not valid MeshRenderer derived component.");
    mBatchesMap.addRenderer(renderer);

    if(renderer->getIsWorldSpace())
    {
        //octree.addOcTreeElement(Ptr<IOcTreeElement>::cast(renderer));
    }
}

void RenderEngine::assignBatch(Ptr<MeshRenderer> renderer)
{
	mBatchesMap.addRenderer(renderer);
}

void RenderEngine::drawLine(const Line& line, f32 thickness /*= 1*/, bool isWorldSpace /*= true*/, Vector4 color /*= Vector4(1,1,1,1)*/)
{
    PROFILER_CPU()
	if (isWorldSpace)
	{
		mShapeBatchRenderer.addLine(line, color);
	}
	else
	{
		mShapeBatchRendererScreenSpace.addLine(line, color);
	}
}

void RenderEngine::drawRectangle(const Rectangle& rectangle, f32 thickness/*= 1*/, bool isWorldSpace /*= true*/, Vector4 color /*= Vector4(1,1,1,1)*/)
{
    PROFILER_CPU()
	const Vector3& leftTopFront = rectangle.getLeftTopFront();
	const Vector3& size = rectangle.getSize();
	drawLine(Line(Vector3(leftTopFront.x, leftTopFront.y, leftTopFront.z), Vector3(leftTopFront.x, leftTopFront.y - size.y, leftTopFront.z)), thickness, isWorldSpace, color);
	drawLine(Line(Vector3(leftTopFront.x, leftTopFront.y - size.y, leftTopFront.z), Vector3(leftTopFront.x + size.x, leftTopFront.y - size.y, leftTopFront.z)), thickness, isWorldSpace, color);
	drawLine(Line(Vector3(leftTopFront.x + size.x, leftTopFront.y - size.y, leftTopFront.z), Vector3(leftTopFront.x + size.x, leftTopFront.y, leftTopFront.z)), thickness, isWorldSpace, color);
	drawLine(Line(Vector3(leftTopFront.x + size.x, leftTopFront.y, leftTopFront.z), Vector3(leftTopFront.x, leftTopFront.y, leftTopFront.z)), thickness, isWorldSpace, color);
}

void RenderEngine::drawCube(const Cube& cube, f32 thickness/*= 1*/, bool isWorldSpace /*= true*/, Vector4 color /*= Vector4(1,1,1,1)*/)
{
    PROFILER_CPU()
	const Vector3& leftTopFront = cube.getLeftTopFront();
	const Vector3& size = cube.getSize();
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
	mShapeBatchRenderer.render();
    
    GET_SYSTEM(RenderContext).clearDepth();
    GET_SYSTEM(RenderContext).clearStencil();
    
	mBatchesMap.renderScreenSpaceStencil();
	mBatchesMap.renderScreenSpace();
	mShapeBatchRendererScreenSpace.render();
}