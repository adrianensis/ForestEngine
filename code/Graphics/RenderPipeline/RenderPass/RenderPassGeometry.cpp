#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"

void RenderPassGeometry::render()
{
	PROFILER_CPU()
    FOR_MAP(it, mBatchMap)
	{
        it->second->render();
	}
}