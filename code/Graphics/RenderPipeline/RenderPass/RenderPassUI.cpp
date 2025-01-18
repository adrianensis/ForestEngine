#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "UI/UIShader.hpp"

void RenderPassUI::init(WeakPtr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData)
{
    RenderPass::init(renderPipeline, renderPassData);
}

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

void RenderPassUI::renderStencilCascade(u64 id)
{    
    FOR_LIST(it, mInstancedMeshRenderers)
	{
        const InstancedMeshData& instancedMeshData = *it;
		if(id == instancedMeshData.mShaderStencilData.mId)
		{
            if(instancedMeshData.mShaderStencilData.mParentId > 0)
            {
                renderStencilCascade(instancedMeshData.mShaderStencilData.mParentId);
            }

            if(!mStencilsRendered.contains(instancedMeshData.mShaderStencilData.mId))
            {
                mStencilsRendered.insert(instancedMeshData.mShaderStencilData.mId);
                renderInstancedMesh(instancedMeshData);
            }

            break;
		}
	}
}

void RenderPassUI::render()
{
	PROFILER_CPU()

    mStencilsRendered.clear();

    std::vector<InstancedMeshData> noStencilInstancedMeshRenderers;
    std::vector<InstancedMeshData> stencilInstancedMeshRenderers;
    FOR_LIST(it, mInstancedMeshRenderers)
	{
        const InstancedMeshData& instancedMeshData = *it;
        WeakPtr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
        if(instancedMeshData.mShaderStencilData.mUseStencil)
        {
            if(instancedMeshData.mShaderStencilData.mParentId > 0)
            {
                stencilInstancedMeshRenderers.push_back(instancedMeshData);
            }
        }
        else
        {
            noStencilInstancedMeshRenderers.push_back(instancedMeshData);
        }
    }

    auto compareStencilBatch = [](InstancedMeshData b1, InstancedMeshData b2)
    {
        u64 o1 = b1.mShaderStencilData.mParentId;
        u64 o2 = b2.mShaderStencilData.mParentId;
        return (o1 < o2);
    };
  
    std::sort(stencilInstancedMeshRenderers.begin(), stencilInstancedMeshRenderers.end(), compareStencilBatch);

    u64 currentId = 0;
    FOR_LIST(it, stencilInstancedMeshRenderers)
	{
        const InstancedMeshData& instancedMeshData = *it;
        if(currentId != instancedMeshData.mShaderStencilData.mParentId)
        {
//            GET_SYSTEM(GPUInterface).clearStencil();
        }

        currentId = instancedMeshData.mShaderStencilData.mParentId;

        renderStencilCascade(instancedMeshData.mShaderStencilData.mId);
	}

//    GET_SYSTEM(GPUInterface).clearStencil();

    FOR_LIST(it, noStencilInstancedMeshRenderers)
	{
        renderInstancedMesh(*it);
    }
}