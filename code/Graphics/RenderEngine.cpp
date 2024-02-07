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
#include "Graphics/GPU/GPUProgram.hpp"
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

	if (mRenderPipelineData.mCamera)
	{
		mRenderPipelineData.mCamera->update();
	}

    std::vector<Ptr<MeshRenderer>> newList;
    FOR_ARRAY(i, mRenderers)
    {
        PROFILER_BLOCK_CPU("remove renderers");

        Ptr<MeshRenderer> renderer = mRenderers[i];
        if(renderer.isValid())
        {
            if (renderer->getIsPendingToBeDestroyed())
            {
                renderer->finallyDestroy();
                GET_SYSTEM(GPUSharedContext).freeInstanceSlot(renderer->getGPUInstanceSlot());
            }
            else
            {
                newList.push_back(renderer);
            }
        }
    }

    mRenderers.clear();
    mRenderers = newList;

    FOR_ARRAY(i, mRenderers)
    {
        PROFILER_BLOCK_CPU("update renderers");
        Ptr<MeshRenderer> renderer = mRenderers[i];
        renderer->update();
        const Matrix4& rendererModelMatrix = renderer->getRendererModelMatrix();
        GET_SYSTEM(GPUSharedContext).setInstanceMatrix(renderer->getGPUInstanceSlot(), rendererModelMatrix);
        GET_SYSTEM(GPUSharedContext).setMaterialInstanceProperties(renderer->getGPUInstanceSlot(), renderer->getMaterialInstance().mMaterialInstancedProperties);
    }

    updateInstancesData();

	GET_SYSTEM(AnimationManager).update();

	// octree.update();

	render();
	swap();
}

void RenderEngine::updateInstancesData()
{
	PROFILER_CPU()
	GET_SYSTEM(GPUSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).setDataArray(GET_SYSTEM(GPUSharedContext).getMatrices());
	GET_SYSTEM(GPUSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(DefaultMaterialInstancedPropertiesGPUData::smDefaultInstancedPropertiesSharedBufferData).setDataArray(GET_SYSTEM(GPUSharedContext).getMaterialInstancedPropertiesArray());
}

void RenderEngine::preSceneChanged()
{
	LOG_TRACE()
}

void RenderEngine::postSceneChanged()
{
	LOG_TRACE()
    mRenderPipelineData.mCamera = GET_SYSTEM(ScenesManager).getCurrentCamera();
}

void RenderEngine::onResize()
{
	LOG_TRACE()
    mRenderPipelineData.mCamera->onResize();
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
        mRenderers.push_back(renderer);

        GPUInstanceSlot slot = GET_SYSTEM(GPUSharedContext).requestInstanceSlot();
        CHECK_MSG(slot.getIsValid(), "Invalid GPUInstanceSlot");
        renderer->setGPUInstanceSlot(slot);

        mBatchesManager.addRenderer(renderer);

        if(renderer->getIsWorldSpace())
        {
            //octree.addOcTreeElement(Ptr<IOcTreeElement>::cast(renderer));
        }
    }
    else if(component->getSystemComponentId() == Light::getClassDefinitionStatic().mId)
    {
        mRenderPipelineData.mLights.push_back(Ptr<Light>::cast(component));
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

void RenderEngine::render()
{
	PROFILER_CPU()

    mRenderPipeline.render(mRenderPipelineData, mBatchesManager, mShapeBatchRenderer, mShapeBatchRendererScreenSpace);
}