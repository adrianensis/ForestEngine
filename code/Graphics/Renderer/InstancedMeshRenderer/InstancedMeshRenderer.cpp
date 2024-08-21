#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/GPU/Mesh/GPUMesh.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Scene/Module.hpp"

void InstancedMeshRenderer::init(const InstancedMeshData& instancedMeshData)
{
    LOG_TRACE()
    PROFILER_CPU()
	mInstancedMeshData = instancedMeshData;

    mRendererSlotsManager.init(mInitialInstances);
    mRenderers.resize(mRendererSlotsManager.getSize());

	mGPUMeshBatcher.init(mInstancedMeshData.mMesh);
    initBuffers();

    resizeMeshBuffers(1);
    setMeshBuffers(mInstancedMeshData.mMesh);
}

void InstancedMeshRenderer::terminate()
{
    mGPUVertexBuffersContainer.terminate();
}

void InstancedMeshRenderer::render()
{
	PROFILER_CPU()
	if (mRenderers.empty()) { return; }
    enable();
    drawCall();
    disable();
}

void InstancedMeshRenderer::enable()
{
    mGPUVertexBuffersContainer.enable();

    if(mInstancedMeshData.mStencilData.mUseStencil)
    {
//        GET_SYSTEM(GPUInterface).enableStencil(mInstancedMeshData.mStencilData.mStencilValue, mInstancedMeshData.mStencilData.mStencilFunction, mInstancedMeshData.mStencilData.mStencilPassOp);
    }
}

void InstancedMeshRenderer::disable()
{
    if(mInstancedMeshData.mStencilData.mUseStencil)
    {
//        GET_SYSTEM(GPUInterface).disableStencil();
    }

    mGPUVertexBuffersContainer.disable();
}

void InstancedMeshRenderer::addRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
    if(mRendererSlotsManager.isEmpty())
    {
        mRendererSlotsManager.increaseSize(mInitialInstances);
        mRenderers.resize(mRendererSlotsManager.getSize());
    }

    renderer->setInstanceSlot(mRendererSlotsManager.requestSlot());
    mRenderers.at(renderer->getInstanceSlot().getSlot()) = renderer;
    mUsedSlots.insert(renderer->getInstanceSlot().getSlot());
	mRegenerateBuffersRequested = true;
    mRenderersCount++;
}

void InstancedMeshRenderer::removeRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
	mRegenerateBuffersRequested = true;
    mRenderers.at(renderer->getInstanceSlot().getSlot()).reset();
    mUsedSlots.erase(renderer->getInstanceSlot().getSlot());
    mRendererSlotsManager.freeSlot(renderer->getInstanceSlot());
    mRenderersCount--;
}

void InstancedMeshRenderer::update()
{
    if (mRenderers.empty()) { return; }
    if (!shouldRegenerateBuffers()) { return; }
	PROFILER_CPU()
    u32 newSize = mRenderersCount;
    if (newSize > mMaxMeshesThreshold)
    {
        PROFILER_BLOCK_CPU(newSize);
		if(mMaxMeshesThreshold == 0)
		{
			mMaxMeshesThreshold = newSize;
		}
		else
		{
			mMaxMeshesThreshold += smMeshesIncrement;
		}

        mGPUMeshBatcher.resize(mMaxMeshesThreshold);
        resizeInstancedBuffers(mMaxMeshesThreshold);
    	resizeIndicesBuffer(mGPUMeshBatcher.getInternalMesh());

        PROFILER_END_BLOCK();
    }

    u32 rendererIndex = 0;
    FOR_RANGE(i, 0, (*mUsedSlots.rbegin())+1)
    {
        TypedComponentHandler<MeshRenderer> renderer = mRenderers[i];
        if(renderer.isValid())
        {
            mGPUMeshBatcher.setInstanceData(rendererIndex, renderer->getRenderSlot().getSlot(), renderer->getMaterialInstance()->mSlot.getSlot());
            rendererIndex++;
        }
    }

    mRegenerateBuffersRequested = false;

    setIndicesBuffer(mGPUMeshBatcher.getInternalMesh());
    setInstancedBuffers();
}

bool InstancedMeshRenderer::shouldRegenerateBuffers() const
{
    // TODO: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mRegenerateBuffersRequested || !mInstancedMeshData.mIsStatic;
}

void InstancedMeshRenderer::initBuffers()
{
    PROFILER_CPU()

    mInstancedMeshData.mMesh->populateGPUVertexBuffersContainer(mGPUVertexBuffersContainer, mInstancedMeshData.mIsStatic);
    mGPUVertexBuffersContainer.create();
    
    mGPUVertexBuffersContainer.enable();
    mGPUVertexBuffersContainer.setIndicesBuffer(GPUBuiltIn::PrimitiveTypes::mFace, mInstancedMeshData.mIsStatic);
    mGPUVertexBuffersContainer.disable();
}

void InstancedMeshRenderer::resizeMeshBuffers(u32 maxInstances)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mInstancedMeshData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mInstancedMeshData.mMesh->mGPUVertexInputBuffers[i];
        mGPUVertexBuffersContainer.getVertexBuffer(gpuVariableData).resize(mInstancedMeshData.mMesh->mVertexCount * maxInstances);
    }
}

void InstancedMeshRenderer::resizeInstancedBuffers(u32 maxInstances)
{
    PROFILER_CPU()
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).resize(maxInstances);
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mObjectID).resize(maxInstances);
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mMaterialInstanceID).resize(maxInstances);
}

void InstancedMeshRenderer::setMeshBuffers(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    FOR_ARRAY(i, mesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mesh->mGPUVertexInputBuffers[i];
        mGPUVertexBuffersContainer.getVertexBuffer(gpuVariableData).setDataArray(mesh->mBuffers.at(gpuVariableData.mName));
    }
}

void InstancedMeshRenderer::setInstancedBuffers()
{
    PROFILER_CPU()
	mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mInstanceID).setDataArray(mGPUMeshBatcher.getInstanceIDs());
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mObjectID).setDataArray(mGPUMeshBatcher.getObjectIDs());
    mGPUVertexBuffersContainer.getVertexBuffer(GPUBuiltIn::VertexInput::mMaterialInstanceID).setDataArray(mGPUMeshBatcher.getMaterialInstanceIDs());
}

void InstancedMeshRenderer::setBonesTransformsBuffer(const std::vector<Matrix4>& transforms)
{
}

void InstancedMeshRenderer::resizeIndicesBuffer(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    mGPUVertexBuffersContainer.getIndicesBuffer().resize(mesh->mIndices.size());
}

void InstancedMeshRenderer::setIndicesBuffer(Ptr<const GPUMesh> mesh)
{
    PROFILER_CPU()
    mGPUVertexBuffersContainer.getIndicesBuffer().setDataArray(mesh->mIndices);
}

void InstancedMeshRenderer::drawCall()
{	
    PROFILER_CPU()
    if(!mRenderers.empty())
    {
//        GET_SYSTEM(GPUInterface).drawElements(GPUDrawPrimitive::TRIANGLES, mInstancedMeshData.mMesh->mIndices.size() * 3, mRenderersCount, true);
    }
}