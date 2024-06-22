#include "Graphics/Renderer/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Scene/Module.hpp"

void BatchRenderer::init(const BatchData& batchData)
{
	mBatchData = batchData;

    mRendererSlotsManager.init(mInitialInstances);
    mRenderers.resize(mRendererSlotsManager.getSize());

	mMeshBatcher.init(mBatchData.mMesh);
    initBuffers();

    resizeMeshBuffers(1);
    setMeshBuffers(mBatchData.mMesh);

    // if(customShader)
    // {
    //     setShader(customShader);
    // }
}

void BatchRenderer::terminate()
{
    mGPUVertexBuffersContainer.terminate();
}

void BatchRenderer::render()
{
	PROFILER_CPU()
	if (!mRenderers.empty())
	{
        enable();
		if(shouldRegenerateBuffers())
		{
			updateBuffers();
		}
        drawCall();
        disable();
	}
}

void BatchRenderer::enable()
{
    // mGPUProgram->enable();
    mGPUVertexBuffersContainer.enable();

    if(mBatchData.mStencilData.mUseStencil)
    {
        GET_SYSTEM(GPUInterface).enableStencil(mBatchData.mStencilData.mStencilValue, mBatchData.mStencilData.mStencilFunction, mBatchData.mStencilData.mStencilPassOp);
    }
}

void BatchRenderer::disable()
{
    if(mBatchData.mStencilData.mUseStencil)
    {
        GET_SYSTEM(GPUInterface).disableStencil();
    }

    mGPUVertexBuffersContainer.disable();
    // mGPUProgram->disable();
}

void BatchRenderer::addRenderer(Ptr<MeshRenderer> renderer)
{
    if(mRendererSlotsManager.isEmpty())
    {
        mRendererSlotsManager.increaseSize(mInitialInstances);
        mRenderers.resize(mRendererSlotsManager.getSize());
    }

    renderer->setBatchSlot(mRendererSlotsManager.requestSlot());
    mRenderers.at(renderer->getBatchSlot().getSlot()) = renderer;
	mRegenerateBuffersRequested = true;
    mRenderersCount++;
}

void BatchRenderer::removeRenderer(Ptr<MeshRenderer> renderer)
{
	mRegenerateBuffersRequested = true;
    mRenderers.at(renderer->getBatchSlot().getSlot()).invalidate();
    mRendererSlotsManager.freeSlot(renderer->getBatchSlot());
    mRenderersCount--;
}

void BatchRenderer::updateBuffers()
{
	PROFILER_CPU()
    u32 newSize = mRenderersCount;
    mMeshBatcher.clear();
    if (newSize > mMaxMeshesThreshold)
    {
        PROFILER_BLOCK_CPU("new size");
		if(mMaxMeshesThreshold == 0)
		{
			mMaxMeshesThreshold = newSize;
		}
		else
		{
			mMaxMeshesThreshold += smMeshesIncrement;
		}

        mMeshBatcher.resize(mMaxMeshesThreshold);
        resizeInstancedBuffers(mMaxMeshesThreshold);
    	resizeIndicesBuffer(mMeshBatcher.getInternalMesh());

        PROFILER_END_BLOCK();
    }

    FOR_ARRAY(i, mRenderers)
    {
        if(mRenderers[i].isValid())
        {
            mMeshBatcher.addInstanceData(mRenderers[i]->getMeshInstance(), mRenderers[i]->getRenderInstanceSlot().getSlot(), mRenderers[i]->getMaterialInstance()->mSlot.getSlot());
        }
    }

    mDataSubmittedToGPU = false;
    mRegenerateBuffersRequested = false;
}

bool BatchRenderer::shouldRegenerateBuffers() const
{
    // TODO: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mRegenerateBuffersRequested || !mBatchData.mIsStatic;
}

void BatchRenderer::updateBoneTransforms()
{
}

void BatchRenderer::initBuffers()
{
    PROFILER_CPU()

    mBatchData.mMesh->populateGPUVertexBuffersContainer(mGPUVertexBuffersContainer, mBatchData.mIsStatic);
    mGPUVertexBuffersContainer.create();
    
    mGPUVertexBuffersContainer.enable();
    mGPUVertexBuffersContainer.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mFace, mBatchData.mIsStatic);
    mGPUVertexBuffersContainer.disable();
}

void BatchRenderer::resizeMeshBuffers(u32 maxInstances)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mBatchData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mBatchData.mMesh->mGPUVertexInputBuffers[i];
        mGPUVertexBuffersContainer.getVertexBuffer(gpuVariableData).resize(mBatchData.mMesh->mVertexCount * maxInstances);
    }
}

void BatchRenderer::resizeInstancedBuffers(u32 maxInstances)
{
    PROFILER_CPU()
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).resize(maxInstances);
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mObjectID).resize(maxInstances);
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mMaterialInstanceID).resize(maxInstances);
}

void BatchRenderer::setMeshBuffers(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mesh->mGPUVertexInputBuffers[i];
        mGPUVertexBuffersContainer.getVertexBuffer(gpuVariableData).setDataArray(mesh->mBuffers.at(gpuVariableData.mName));
    }
}

void BatchRenderer::setInstancedBuffers()
{
    PROFILER_CPU()
	mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).setDataArray(mMeshBatcher.getInstanceIDs());
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mObjectID).setDataArray(mMeshBatcher.getObjectIDs());
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mMaterialInstanceID).setDataArray(mMeshBatcher.getMaterialInstanceIDs());
}

void BatchRenderer::setBonesTransformsBuffer(const std::vector<Matrix4>& transforms)
{
}

void BatchRenderer::resizeIndicesBuffer(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    mGPUVertexBuffersContainer.getIndicesBuffer().resize(mesh->mIndices.size());
}

void BatchRenderer::setIndicesBuffer(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    mGPUVertexBuffersContainer.getIndicesBuffer().setDataArray(mesh->mIndices);
}

void BatchRenderer::drawCall()
{	
    PROFILER_CPU()
    if(!mRenderers.empty())
    {
        if(!mDataSubmittedToGPU)
        {
            setIndicesBuffer(mMeshBatcher.getInternalMesh());
            setInstancedBuffers();
            mDataSubmittedToGPU = true;
        }

        GET_SYSTEM(GPUInterface).drawElements(GPUDrawPrimitive::TRIANGLES, mBatchData.mMesh->mIndices.size() * 3, mRenderersCount, true);
    }
}