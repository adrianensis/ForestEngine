#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Scene/Module.hpp"

void RenderPipeline::init()
{
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

        if(!mBatchMap.contains(*it))
        {
            mBatchMap.insert_or_assign(*it, BatchMap());
        }
        
        BatchData batchData;
	    batchData.init(renderer);
        if(!mBatchMap.at(*it).contains(batchData))
        {
            mBatchMap.at(*it).insert_or_assign(batchData, OwnerPtr<BatchRenderer>::newObject());
            mBatchMap.at(*it).at(batchData)->init(batchData);
        }

        mBatchMap.at(*it).at(batchData)->addRenderer(renderer);
    }
}

void RenderPipeline::removeRenderer(Ptr<MeshRenderer> renderer)
{
    FOR_LIST(it, renderer->getRendererData().mRenderPassIDs)
    {
        CHECK_MSG(mRenderPassMap.contains(*it), "RenderPass not found!");
        mRenderPassMap.at(*it)->removeRenderer(renderer);

        BatchData batchData;
	    batchData.init(renderer);
        // if(!mBatchMap.contains(*it))
        // if(!mBatchMap.at(*it).contains(batchData))
        mBatchMap.at(*it).at(batchData)->removeRenderer(renderer);
    }
}

void RenderPipeline::render(RenderPipelineData& renderData)
{
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

    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mLightsBufferData).setData(lightsData);
}