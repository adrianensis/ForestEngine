#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"
#include "GPU/GPUInstance.hpp"
#include "Scene/Module.hpp"

void InstancedMeshRenderer::init(const InstancedMeshData& instancedMeshData)
{
    LOG_TRACE()
    PROFILER_CPU()
	mInstancedMeshData = instancedMeshData;

    mRendererSlotsManager.init(smInitialInstancesSize);
    mRenderers.resize(mRendererSlotsManager.getSize());

	mGPUMeshBatcher.init(mInstancedMeshData.mMesh, smInitialInstancesSize);

    FOR_ARRAY(i, mInstancedMeshData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mInstancedMeshData.mMesh->mGPUVertexInputBuffers[i];
        GPUVertexBufferData bufferData(gpuVariableData);
        // mVertexCount * 1 = only 1 instance
        mGPUVertexBuffersContainer.addVertexBuffer(bufferData, mInstancedMeshData.mMesh->mVertexCount * 1, mInstancedMeshData.mIsStatic);
        mGPUVertexBuffersContainer.getVertexBuffer(bufferData).setDataArray(mInstancedMeshData.mMesh->mBuffers.at(bufferData.mGPUVariableData.mName));
    }

    GPUVertexBufferData bufferDataInstanceIDs(GPUShaderDefinitions::VertexInput::mInstanceID, 1);
    mGPUVertexBuffersContainer.addVertexBuffer(bufferDataInstanceIDs, smInitialInstancesSize, mInstancedMeshData.mIsStatic);
    GPUVertexBufferData bufferDataObjectIDs(GPUShaderDefinitions::VertexInput::mObjectID, 1);
    mGPUVertexBuffersContainer.addVertexBuffer(bufferDataObjectIDs, smInitialInstancesSize, mInstancedMeshData.mIsStatic);
    GPUVertexBufferData bufferDataShaderPropertiesInstanceIDs(GPUShaderDefinitions::VertexInput::mShaderPropertiesInstanceID, 1);
    mGPUVertexBuffersContainer.addVertexBuffer(bufferDataShaderPropertiesInstanceIDs, smInitialInstancesSize, mInstancedMeshData.mIsStatic);

    mGPUVertexBuffersContainer.setIndicesBuffer(GPUShaderDefinitions::PrimitiveTypes::mFace, mGPUMeshBatcher.getInternalMesh()->mIndices.size(), mInstancedMeshData.mIsStatic);
    mGPUVertexBuffersContainer.getIndicesBuffer().setDataArray(mGPUMeshBatcher.getInternalMesh()->mIndices);
    
    mCurrentInstancesSize = smInitialInstancesSize;
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

    if(mInstancedMeshData.mShaderStencilData.mUseStencil)
    {
//        GET_SYSTEM(GPUInterface).enableStencil(mInstancedMeshData.mShaderStencilData.mStencilValue, mInstancedMeshData.mShaderStencilData.mStencilFunction, mInstancedMeshData.mShaderStencilData.mStencilPassOp);
    }
}

void InstancedMeshRenderer::disable()
{
    if(mInstancedMeshData.mShaderStencilData.mUseStencil)
    {
//        GET_SYSTEM(GPUInterface).disableStencil();
    }

    // mGPUVertexBuffersContainer.disable();
}

void InstancedMeshRenderer::addRenderer(TComponentHandler<MeshRenderer> renderer)
{
    PROFILER_CPU_NAMED(add_renderer)
    if(mRendererSlotsManager.isEmpty())
    {
        mRendererSlotsManager.increaseSize(smInitialInstancesSize);
        mRenderers.resize(mRendererSlotsManager.getSize());
    }

    renderer->setInstanceSlot(mRendererSlotsManager.requestSlot());
    mRenderers.at(renderer->getInstanceSlot().getSlot()) = renderer;
    mUsedSlots.insert(renderer->getInstanceSlot().getSlot());
	mResizeBuffersRequested = true;
    mRenderersCount++;
}

void InstancedMeshRenderer::removeRenderer(TComponentHandler<MeshRenderer> renderer)
{
	mResizeBuffersRequested = true;
    mRenderers.at(renderer->getInstanceSlot().getSlot()).reset();
    mUsedSlots.erase(renderer->getInstanceSlot().getSlot());
    mRendererSlotsManager.freeSlot(renderer->getInstanceSlot());
    mRenderersCount--;
}

void InstancedMeshRenderer::update(VkCommandBuffer commandBuffer)
{
    if (mRenderers.empty())
    {
        return;
    }
    if (!shouldResize())
    {
        return;
    }
    if (mRenderersCount == 0)
    {
        return;
    }
	PROFILER_CPU()
    u32 newSize = mRenderersCount;
    if (newSize > mCurrentInstancesSize)
    {
        PROFILER_CPU_NAMED(InstancedBuffers_Resize)

		mCurrentInstancesSize += smInstancesSizeIncrement;

        mGPUMeshBatcher.resize(mCurrentInstancesSize);
        
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mInstanceID).resize(mCurrentInstancesSize);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mObjectID).resize(mCurrentInstancesSize);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mShaderPropertiesInstanceID).resize(mCurrentInstancesSize);
    }

    u32 rendererIndex = 0;
    FOR_RANGE(i, 0, (*mUsedSlots.rbegin())+1)
    {
        TComponentHandler<MeshRenderer> renderer = mRenderers[i];
        if(renderer.isValid())
        {
            mGPUMeshBatcher.setInstanceData(rendererIndex, renderer->getRenderSlot().getSlot(), renderer->getShaderPropertiesInstance()->mSlot.getSlot());
            rendererIndex++;
        }
    }

    {
        PROFILER_CPU_NAMED(InstancedBuffers_SetData)
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mInstanceID).setDataArray(mGPUMeshBatcher.getInstanceIDs(), commandBuffer);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mObjectID).setDataArray(mGPUMeshBatcher.getObjectIDs(), commandBuffer);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mShaderPropertiesInstanceID).setDataArray(mGPUMeshBatcher.getShaderPropertiesInstanceIDs(), commandBuffer);
    }

    mResizeBuffersRequested = false;
}

void InstancedMeshRenderer::setBonesTransformsBuffer(const std::vector<Matrix4>& transforms)
{
}

bool InstancedMeshRenderer::shouldResize() const
{
    // PERF: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mResizeBuffersRequested || !mInstancedMeshData.mIsStatic;
}

void InstancedMeshRenderer::drawCall()
{	
    PROFILER_CPU()
    if(!mRenderers.empty())
    {
//        GET_SYSTEM(GPUInterface).drawElements(GPUDrawPrimitive::TRIANGLES, mInstancedMeshData.mMesh->mIndices.size() * 3, mRenderersCount, true);
        u32 instanceCount = mRenderersCount;
        constexpr u32 firstVertex = 0;
        constexpr u32 vertexOffset = 0;
        constexpr u32 firstInstance = 0;
        const GPUCommandBuffer& vulkanCommandBuffer = GET_SYSTEM(GPUInstance).mGPUContext->vulkanCommandBuffers[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
        GET_SYSTEM(GPUInstance).mGPUContext->drawIndexed(vulkanCommandBuffer.getVkCommandBuffer(), mInstancedMeshData.mMesh->mIndices.size() * 3, instanceCount, firstVertex, vertexOffset, firstInstance);
    }
}