#include "GPU/RenderGraph/GPURenderGraph.hpp"
#include "GPU/GPUInstance.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "GPU/GPULight.hpp"

void GPURenderGraph::init(Ptr<GPUContext> gpuContext, WeakPtr<GPUInstanceRendererManager> gpuInstanceRendererManager)
{
    PROFILER_CPU()
    mGPUContext = gpuContext;
    mGPUInstanceRendererManager = gpuInstanceRendererManager;
}

void GPURenderGraph::render(GPURenderGraphData& renderData)
{
}

void GPURenderGraph::update()
{
	PROFILER_CPU()

}

void GPURenderGraph::terminate()
{
    FOR_MAP(it, mRenderPassMap)
	{
        it->second->terminate();
	}
}

void GPURenderGraph::onResize()
{
    FOR_MAP(it, mRenderPassMap)
	{
        it->second->onResize();
	}
}

void GPURenderGraph::addRenderer(WeakPtr<GPURenderItem> renderItem)
{
    PROFILER_CPU()
    GPUInstanceRendererData gpuInstanceRendererData;
    gpuInstanceRendererData.init(renderItem);

    FOR_LIST(it, renderItem->getGPURenderItemData().mRenderPassIDs)
    {
        if(mRenderPassMap.contains(*it))
        {
            Ptr<GPURenderPass> renderPass = mRenderPassMap.at(*it);
            renderPass->addInstanceRendererData(gpuInstanceRendererData);
        }
    }
}

void GPURenderGraph::removeRenderer(WeakPtr<GPURenderItem> renderItem)
{
    PROFILER_CPU()
    GPUInstanceRendererData gpuInstanceRendererData;
    gpuInstanceRendererData.init(renderItem);

    FOR_LIST(it, renderItem->getGPURenderItemData().mRenderPassIDs)
    {
        if(mRenderPassMap.contains(*it))
        {
            // TODO: Only remove if renderers count == 0
            // renderPass->getGPUInstanceRendererRegistry().removeInstanceRendererData(gpuInstanceRendererData);
        }
    }
}

void GPURenderGraph::updateLights(GPURenderGraphData& renderData)
{
	PROFILER_CPU()

    // GPULightBuiltIn::LightsData lightsData;
    // FOR_ARRAY(i, renderData.mPointLights)
    // {
    //     lightsData.mPointLights[i] = renderData.mPointLights[i]->calculateLightData();
    // }

    // if(renderData.mDirectionalLight)
    // {
    //     lightsData.mDirectionalLight = renderData.mDirectionalLight->calculateLightData();
    // }

    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mLightsBufferData).setData(lightsData);
}