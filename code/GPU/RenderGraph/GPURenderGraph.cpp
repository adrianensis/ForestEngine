#include "GPU/RenderGraph/GPURenderGraph.hpp"

void GPURenderGraph::init(Ptr<GPUContext> gpuContext)
{
    PROFILER_CPU()
    mGPUContext = gpuContext;
}

void GPURenderGraph::update()
{
	PROFILER_CPU()
}

void GPURenderGraph::terminate()
{
    // TODO: delete mRenderPassMap content?
}

void GPURenderGraph::onResize()
{
    // FOR_MAP(it, mRenderPassMap)
	// {
    //     it->second->onResize();
	// }
}

void GPURenderGraph::render(GPURenderGraphData& renderData)
{
}