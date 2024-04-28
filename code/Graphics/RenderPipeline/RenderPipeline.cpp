#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/BatchRenderer/ShapeBatchRenderer.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void RenderPipeline::init()
{
    mRenderPassMap.insert_or_assign(
        ClassManager::getClassMetadata<RenderPassGeometry>().mClassDefinition.mId,
        OwnerPtr<RenderPass>::moveCast(OwnerPtr<RenderPassGeometry>::newObject())
    );
    mRenderPassMap.insert_or_assign(
        ClassManager::getClassMetadata<RenderPassUI>().mClassDefinition.mId,
        OwnerPtr<RenderPass>::moveCast(OwnerPtr<RenderPassUI>::newObject())
    );
    mRenderPassMap.insert_or_assign(
        ClassManager::getClassMetadata<RenderPassUIStencil>().mClassDefinition.mId,
        OwnerPtr<RenderPass>::moveCast(OwnerPtr<RenderPassUIStencil>::newObject())
    );

    FOR_MAP(it, mRenderPassMap)
	{
        it->second->init();
	}
}

void RenderPipeline::terminate()
{
    FOR_MAP(it, mRenderPassMap)
	{
        it->second->terminate();
	}
}

void RenderPipeline::addRenderer(Ptr<MeshRenderer> renderer)
{
    FOR_LIST(it, renderer->getRendererData().mRenderPassIDs)
    {
        CHECK_MSG(mRenderPassMap.contains(*it), "RenderPass not found!");
        mRenderPassMap.at(*it)->addRenderer(renderer);
    }
}

void RenderPipeline::removeRenderer(Ptr<MeshRenderer> renderer)
{
    FOR_LIST(it, renderer->getRendererData().mRenderPassIDs)
    {
        CHECK_MSG(mRenderPassMap.contains(*it), "RenderPass not found!");
        mRenderPassMap.at(*it)->removeRenderer(renderer);
    }
}

void RenderPipeline::render(RenderPipelineData& renderData)
{
	PROFILER_CPU()

    updateLights(renderData);

    updateGlobalData(renderData, true);

	GET_SYSTEM(GPUInterface).clear();

    PROFILER_BLOCK_CPU("render");
    renderRenderPass<RenderPassGeometry>();

    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("render shapes");
	GET_SYSTEM(DebugRenderer).mShapeBatchRenderer.render();
    PROFILER_END_BLOCK();
    
    GET_SYSTEM(GPUInterface).clearDepth();
    GET_SYSTEM(GPUInterface).clearStencil();

    updateGlobalData(renderData, false);
    
    PROFILER_BLOCK_CPU("renderScreenSpaceStencil");
    renderRenderPass<RenderPassUIStencil>();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("renderScreenSpace");
    renderRenderPass<RenderPassUI>();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("renderScreenSpace shapes");
	GET_SYSTEM(DebugRenderer).mShapeBatchRendererScreenSpace.render();
    PROFILER_END_BLOCK();
}

void RenderPipeline::updateGlobalData(RenderPipelineData& renderData, bool isWorldSpace)
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuGlobalData =
    {
        isWorldSpace ? renderData.mCamera->mProjectionMatrix : ortho,
        isWorldSpace ? renderData.mCamera->mViewMatrix : Matrix4::smIdentity,
        renderData.mCamera->mGameObject->mTransform->getWorldPosition(),
        GET_SYSTEM(Time).getDeltaTimeSeconds(),
        GET_SYSTEM(Window).getWindowSize()
    };
	GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).setData(gpuGlobalData);
}

void RenderPipeline::updateLights(RenderPipelineData& renderData)
{
	PROFILER_CPU()

    LightBuiltIn::LightsData lightsData;
    FOR_ARRAY(i, renderData.mPointLights)
    {
        lightsData.mPointLights[i] = renderData.mPointLights[i]->getLightData();
    }

    if(renderData.mDirectionalLight)
    {
        lightsData.mDirectionalLight = renderData.mDirectionalLight->getLightData();
    }

    GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mLightsBufferData).setData(lightsData);
}