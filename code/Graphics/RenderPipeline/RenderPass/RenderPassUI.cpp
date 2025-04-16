#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"

void RenderPassUI::preRender()
{
//    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::MULTISAMPLE);
//    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::DEPTH_TEST);
//    GET_SYSTEM(GPUInterface).setDepthFunc(GPUDepthFunc::LEQUAL);
//    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::BLEND);
//    GET_SYSTEM(GPUInterface).setBlendFunc(GPUBlendFactor::SRC_ALPHA, GPUBlendFactor::ONE_MINUS_SRC_ALPHA);
}

void RenderPassUI::postRender()
{
//    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::BLEND);
//    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::DEPTH_TEST);
//    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::MULTISAMPLE);
}

void RenderPassUI::renderStencilCascade(u64 id, const std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>& gpuInstanceRendererDataByRenderPass)
{    
    FOR_LIST(it, gpuInstanceRendererDataByRenderPass)
	{
        const GPUInstanceRendererData& gpuInstanceRendererData = *it;
		if(id == gpuInstanceRendererData.mGPUShaderStencilData.mId)
		{
            if(gpuInstanceRendererData.mGPUShaderStencilData.mParentId > 0)
            {
                renderStencilCascade(gpuInstanceRendererData.mGPUShaderStencilData.mParentId, gpuInstanceRendererDataByRenderPass);
            }

            if(!mStencilsRendered.contains(gpuInstanceRendererData.mGPUShaderStencilData.mId))
            {
                mStencilsRendered.insert(gpuInstanceRendererData.mGPUShaderStencilData.mId);
                renderGPUInstanceRenderer(gpuInstanceRendererData);
            }

            break;
		}
	}
}

void RenderPassUI::render(const std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>& gpuInstanceRendererDataByRenderPass)
{
	PROFILER_CPU()

    mStencilsRendered.clear();

    std::vector<GPUInstanceRendererData> noStencilGPUInstanceRendererRenderers;
    std::vector<GPUInstanceRendererData> stencilGPUInstanceRendererRenderers;
    FOR_LIST(it, gpuInstanceRendererDataByRenderPass)
	{
        const GPUInstanceRendererData& gpuInstanceRendererData = *it;
        WeakPtr<GPUInstanceRenderer> gpuInstanceRenderer = mRenderPipeline->getGPUInstanceRendereresMap().at(gpuInstanceRendererData);
        if(gpuInstanceRendererData.mGPUShaderStencilData.mUseStencil)
        {
            if(gpuInstanceRendererData.mGPUShaderStencilData.mParentId > 0)
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
        u64 o1 = b1.mGPUShaderStencilData.mParentId;
        u64 o2 = b2.mGPUShaderStencilData.mParentId;
        return (o1 < o2);
    };
  
    std::sort(stencilGPUInstanceRendererRenderers.begin(), stencilGPUInstanceRendererRenderers.end(), compareStencilBatch);

    u64 currentId = 0;
    FOR_LIST(it, stencilGPUInstanceRendererRenderers)
	{
        const GPUInstanceRendererData& gpuInstanceRendererData = *it;
        if(currentId != gpuInstanceRendererData.mGPUShaderStencilData.mParentId)
        {
//            GET_SYSTEM(GPUInterface).clearStencil();
        }

        currentId = gpuInstanceRendererData.mGPUShaderStencilData.mParentId;

        renderStencilCascade(gpuInstanceRendererData.mGPUShaderStencilData.mId, gpuInstanceRendererDataByRenderPass);
	}

//    GET_SYSTEM(GPUInterface).clearStencil();

    FOR_LIST(it, noStencilGPUInstanceRendererRenderers)
	{
        renderGPUInstanceRenderer(*it);
    }
}