#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"

void RenderPassUI::preRender()
{
//    (GPUInterface).enableFlag(GPUFlags::MULTISAMPLE);
//    (GPUInterface).enableFlag(GPUFlags::DEPTH_TEST);
//    (GPUInterface).setDepthFunc(GPUDepthFunc::LEQUAL);
//    (GPUInterface).enableFlag(GPUFlags::BLEND);
//    (GPUInterface).setBlendFunc(GPUBlendFactor::SRC_ALPHA, GPUBlendFactor::ONE_MINUS_SRC_ALPHA);
}

void RenderPassUI::postRender()
{
//    (GPUInterface).disableFlag(GPUFlags::BLEND);
//    (GPUInterface).disableFlag(GPUFlags::DEPTH_TEST);
//    (GPUInterface).disableFlag(GPUFlags::MULTISAMPLE);
}

void RenderPassUI::renderStencilCascade(Core::u64 id)
{    
    FOR_LIST(it, mGPUInstanceRendererRegistry.getGPUInstanceRendererDataSet())
	{
        const GPUInstanceRendererData& gpuInstanceRendererData = *it;
		if(id == gpuInstanceRendererData.mGPUDepthStencilData.mId)
		{
            if(gpuInstanceRendererData.mGPUDepthStencilData.mParentId > 0)
            {
                renderStencilCascade(gpuInstanceRendererData.mGPUDepthStencilData.mParentId);
            }

            if(!mStencilsRendered.contains(gpuInstanceRendererData.mGPUDepthStencilData.mId))
            {
                mStencilsRendered.insert(gpuInstanceRendererData.mGPUDepthStencilData.mId);
                renderGPUInstanceRenderer(gpuInstanceRendererData);
            }

            break;
		}
	}
}

void RenderPassUI::render()
{
	PROFILER_CPU()

    mStencilsRendered.clear();

    std::vector<GPUInstanceRendererData> noStencilGPUInstanceRendererRenderers;
    std::vector<GPUInstanceRendererData> stencilGPUInstanceRendererRenderers;
    FOR_LIST(it, mGPUInstanceRendererRegistry.getGPUInstanceRendererDataSet())
	{
        const GPUInstanceRendererData& gpuInstanceRendererData = *it;
        const GPUInstanceRenderer* gpuInstanceRenderer = mGPURenderPassSubsystems.mGPUInstanceRendererManager->getInstanceRenderer(gpuInstanceRendererData);
        if(gpuInstanceRendererData.mGPUDepthStencilData.mStencilEnable)
        {
            if(gpuInstanceRendererData.mGPUDepthStencilData.mParentId > 0)
            {
                stencilGPUInstanceRendererRenderers.push_back(gpuInstanceRendererData);
            }
        }
        else
        {
            noStencilGPUInstanceRendererRenderers.push_back(gpuInstanceRendererData);
        }
    }

    auto compareStencilBatch = [](GPUInstanceRendererData b1, GPUInstanceRendererData b2)
    {
        Core::u64 o1 = b1.mGPUDepthStencilData.mParentId;
        Core::u64 o2 = b2.mGPUDepthStencilData.mParentId;
        return (o1 < o2);
    };
  
    std::sort(stencilGPUInstanceRendererRenderers.begin(), stencilGPUInstanceRendererRenderers.end(), compareStencilBatch);

    Core::u64 currentId = 0;
    FOR_LIST(it, stencilGPUInstanceRendererRenderers)
	{
        const GPUInstanceRendererData& gpuInstanceRendererData = *it;
        if(currentId != gpuInstanceRendererData.mGPUDepthStencilData.mParentId)
        {
//            (GPUInterface).clearStencil();
        }

        currentId = gpuInstanceRendererData.mGPUDepthStencilData.mParentId;

        renderStencilCascade(gpuInstanceRendererData.mGPUDepthStencilData.mId);
	}

//    (GPUInterface).clearStencil();

    FOR_LIST(it, noStencilGPUInstanceRendererRenderers)
	{
        renderGPUInstanceRenderer(*it);
    }
}