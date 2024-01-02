#include "Graphics/RenderEngine.hpp"
#include "Graphics/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/Camera/Frustum.hpp"
#include "Graphics/GPU/GPUShader.hpp"
#include "Scene/Module.hpp"
#include "Engine/EngineConfig.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"

void RenderEngine::init()
{
	LOG_TRACE()

	registerComponentClass(MeshRenderer::getClassDefinitionStatic().mId);
	registerComponentClass(Light::getClassDefinitionStatic().mId);

	mCameraDirtyTranslation = true;

	mShapeBatchRenderer.init(true, 2);
	mShapeBatchRendererScreenSpace.init(false, 2);

	// octree.init(5000);
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

	GET_SYSTEM(GPUInterface).clear();
	renderBatches();
	swap();
}

void RenderEngine::preSceneChanged()
{
	LOG_TRACE()
}

void RenderEngine::postSceneChanged()
{
	LOG_TRACE()
}

void RenderEngine::terminate()
{
	LOG_TRACE()
    
    mBatchesManager.terminate();
	mShapeBatchRenderer.terminate();
	mShapeBatchRendererScreenSpace.terminate();
}

void RenderEngine::addComponent(Ptr<SystemComponent> component)
{
	System::addComponent(component);

    if(component->getSystemComponentId() == MeshRenderer::getClassDefinitionStatic().mId)
    {
        Ptr<MeshRenderer> renderer = Ptr<MeshRenderer>::cast(component);
        mBatchesManager.addRenderer(renderer);

        if(renderer->getIsWorldSpace())
        {
            //octree.addOcTreeElement(Ptr<IOcTreeElement>::cast(renderer));
        }
    }
    else if(component->getSystemComponentId() == Light::getClassDefinitionStatic().mId)
    {
        mLights.push_back(Ptr<Light>::cast(component));
    }
    else
    {
        CHECK_MSG(false, "Trying to add a not valid component.");
    }
}

void RenderEngine::assignBatch(Ptr<MeshRenderer> renderer)
{
	mBatchesManager.addRenderer(renderer);
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

	GET_SYSTEM(Window).swap();
}

void RenderEngine::updateGPUSharedContext(bool isWorldSpace)
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuMatricesData =
    {
        isWorldSpace ? mCamera->mProjectionMatrix : ortho,
        isWorldSpace ? mCamera->mViewMatrix : Matrix4::smIdentity,
        mCamera->mGameObject->mTransform->getWorldPosition()
    };
	GET_SYSTEM(GPUSharedContext).mGlobalDataBuffer.setData(gpuMatricesData);

    GPULightsData gpuLightsData;
    FOR_ARRAY(i, mLights)
    {
        gpuLightsData.mLights[i] = mLights[i]->getLightData();
    }

    GET_SYSTEM(GPUSharedContext).mLightDataBuffer.setData(gpuLightsData);
}
void RenderEngine::renderBatches()
{
	PROFILER_CPU()

    updateGPUSharedContext(true);

    PROFILER_BLOCK_CPU("renderStencil");
	mBatchesManager.renderStencil();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("render");
	mBatchesManager.render();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("render");
	mShapeBatchRenderer.render();
    PROFILER_END_BLOCK();
    
    GET_SYSTEM(GPUInterface).clearDepth();
    GET_SYSTEM(GPUInterface).clearStencil();

    updateGPUSharedContext(false);
    
    PROFILER_BLOCK_CPU("renderScreenSpaceStencil");
	mBatchesManager.renderScreenSpaceStencil();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("renderScreenSpace");
	mBatchesManager.renderScreenSpace();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("render");
	mShapeBatchRendererScreenSpace.render();
    PROFILER_END_BLOCK();
}