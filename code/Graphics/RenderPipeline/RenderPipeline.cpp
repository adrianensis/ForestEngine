#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/BatchRenderer/BatchesManager.hpp"
#include "Graphics/BatchRenderer/ShapeBatchRenderer.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Window/Window.hpp"

void RenderPipeline::render(RenderPipelineData& renderData, BatchesManager& batchesManager, ShapeBatchRenderer& shapeBatchRenderer, ShapeBatchRenderer& shapeBatchRendererScreenSpace)
{
	PROFILER_CPU()

    updateLights(renderData);

    updateGlobalMatrices(renderData, true);

	GET_SYSTEM(GPUInterface).clear();

    PROFILER_BLOCK_CPU("renderStencil");
	batchesManager.renderStencil();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("render");
	batchesManager.render();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("render shapes");
	shapeBatchRenderer.render();
    PROFILER_END_BLOCK();
    
    GET_SYSTEM(GPUInterface).clearDepth();
    GET_SYSTEM(GPUInterface).clearStencil();

    updateGlobalMatrices(renderData, false);
    
    PROFILER_BLOCK_CPU("renderScreenSpaceStencil");
	batchesManager.renderScreenSpaceStencil();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("renderScreenSpace");
	batchesManager.renderScreenSpace();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("renderScreenSpace shapes");
	shapeBatchRendererScreenSpace.render();
    PROFILER_END_BLOCK();
}

void RenderPipeline::updateGlobalMatrices(RenderPipelineData& renderData, bool isWorldSpace)
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuMatricesData =
    {
        isWorldSpace ? renderData.mCamera->mProjectionMatrix : ortho,
        isWorldSpace ? renderData.mCamera->mViewMatrix : Matrix4::smIdentity,
        renderData.mCamera->mGameObject->mTransform->getWorldPosition(),
        GET_SYSTEM(Time).getDeltaTimeSeconds(),
        GET_SYSTEM(Window).getWindowSize()
    };
	GET_SYSTEM(GPUSharedContext).mGlobalDataBuffer.setData(gpuMatricesData);
}

void RenderPipeline::updateLights(RenderPipelineData& renderData)
{
	PROFILER_CPU()

    GPULightsData gpuLightsData;
    FOR_ARRAY(i, renderData.mLights)
    {
        gpuLightsData.mLights[i] = renderData.mLights[i]->getLightData();
    }

    GET_SYSTEM(GPUSharedContext).mLightDataBuffer.setData(gpuLightsData);
}