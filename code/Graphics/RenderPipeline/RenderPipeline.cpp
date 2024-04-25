#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/BatchRenderer/BatchesManager.hpp"
#include "Graphics/BatchRenderer/ShapeBatchRenderer.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"

void RenderPipeline::render(RenderPipelineData& renderData, BatchesManager& batchesManager)
{
	PROFILER_CPU()

    updateLights(renderData);

    updateGlobalData(renderData, true);

	GET_SYSTEM(GPUInterface).clear();

    PROFILER_BLOCK_CPU("renderStencil");
	batchesManager.renderStencil();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("render");
	batchesManager.render();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("render shapes");
	GET_SYSTEM(DebugRenderer).mShapeBatchRenderer.render();
    PROFILER_END_BLOCK();
    
    GET_SYSTEM(GPUInterface).clearDepth();
    GET_SYSTEM(GPUInterface).clearStencil();

    updateGlobalData(renderData, false);
    
    PROFILER_BLOCK_CPU("renderScreenSpaceStencil");
	batchesManager.renderScreenSpaceStencil();
    PROFILER_END_BLOCK();
    PROFILER_BLOCK_CPU("renderScreenSpace");
	batchesManager.renderScreenSpace();
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
    FOR_ARRAY(i, renderData.mLights)
    {
        lightsData.mLights[i] = renderData.mLights[i]->getLightData();
    }

    GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mLightsBufferData).setData(lightsData);
}