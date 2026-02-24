#include "GPU/InstanceRenderer/GPUInstanceRenderer.hpp"
#include "GPU/Shader/GPUShaderDefinitions.hpp"

void GPUInstanceRenderer::init(GPUContext* gpuContext, const GPUInstanceRendererData& gpuInstanceRendererData)
{
    PROFILER_CPU()
    mGPUContext = gpuContext;
	mGPUInstanceRendererData = gpuInstanceRendererData;

    mRendererSlotsManager.init(smInitialInstancesSize);
    mRenderers.resize(mRendererSlotsManager.getSize());

	mGPUMeshBatcher.init(mGPUInstanceRendererData.mMesh, smInitialInstancesSize);

    FOR_ARRAY(i, mGPUInstanceRendererData.mMesh->mGPUVertexInputBuffers)
    {
        const GPUVariableData& gpuVariableData = mGPUInstanceRendererData.mMesh->mGPUVertexInputBuffers[i];
        GPUVertexBufferData bufferData(gpuVariableData);
        // mVertexCount * 1 = only 1 instance
        mGPUVertexBuffersContainer.addVertexBuffer(mGPUContext, bufferData, mGPUInstanceRendererData.mMesh->mVertexCount * 1, mGPUInstanceRendererData.mIsStatic);
        mGPUVertexBuffersContainer.getVertexBuffer(bufferData).setDataArray(mGPUInstanceRendererData.mMesh->mBuffers.at(bufferData.mGPUVariableData.mName));
    }

    GPUVertexBufferData bufferDataInstanceIDs(GPUShaderDefinitions::VertexInput::mInstanceID, 1);
    mGPUVertexBuffersContainer.addVertexBuffer(mGPUContext, bufferDataInstanceIDs, smInitialInstancesSize, mGPUInstanceRendererData.mIsStatic);
    GPUVertexBufferData bufferDataObjectIDs(GPUShaderDefinitions::VertexInput::mObjectID, 1);
    mGPUVertexBuffersContainer.addVertexBuffer(mGPUContext, bufferDataObjectIDs, smInitialInstancesSize, mGPUInstanceRendererData.mIsStatic);
    GPUVertexBufferData bufferDataGPUShaderPropertiesInstanceIDs(GPUShaderDefinitions::VertexInput::mGPUShaderPropertiesInstanceID, 1);
    mGPUVertexBuffersContainer.addVertexBuffer(mGPUContext, bufferDataGPUShaderPropertiesInstanceIDs, smInitialInstancesSize, mGPUInstanceRendererData.mIsStatic);

    mGPUVertexBuffersContainer.setIndicesBuffer(mGPUContext, GPUShaderDefinitions::PrimitiveTypes::mFace, mGPUMeshBatcher.getInternalMesh()->mIndices.size(), mGPUInstanceRendererData.mIsStatic);
    mGPUVertexBuffersContainer.getIndicesBuffer().setDataArray(mGPUMeshBatcher.getInternalMesh()->mIndices);
    
    mCurrentInstancesSize = smInitialInstancesSize;
}

void GPUInstanceRenderer::terminate()
{
    mGPUVertexBuffersContainer.terminate();
}

void GPUInstanceRenderer::render()
{
	PROFILER_CPU()
	if (mRenderers.empty()) { return; }
    enable();
    drawCall();
    disable();
}

void GPUInstanceRenderer::enable()
{
    mGPUVertexBuffersContainer.enable(mGPUContext);
}

void GPUInstanceRenderer::disable()
{

}

void GPUInstanceRenderer::addRenderer(Core::WeakPtr<GPURenderItem> renderItem)
{
    PROFILER_CPU_NAMED(add_renderer)
    if(mRendererSlotsManager.isEmpty())
    {
        mRendererSlotsManager.increaseSize(smInitialInstancesSize);
        mRenderers.resize(mRendererSlotsManager.getSize());
    }

    renderItem->setInstanceSlot(mRendererSlotsManager.requestSlot());
    mRenderers.at(renderItem->getInstanceSlot().getSlot()) = renderItem;
    mUsedSlots.insert(renderItem->getInstanceSlot().getSlot());
	mResizeBuffersRequested = true;
    mRenderersCount++;
}

void GPUInstanceRenderer::removeRenderer(Core::WeakPtr<GPURenderItem> renderItem)
{
	mResizeBuffersRequested = true;
    // mRenderers.at(renderer->getInstanceSlot().getSlot()).reset();
    mUsedSlots.erase(renderItem->getInstanceSlot().getSlot());
    mRendererSlotsManager.freeSlot(renderItem->getInstanceSlot());
    mRenderersCount--;
}

void GPUInstanceRenderer::update(VkCommandBuffer commandBuffer)
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
    Core::u32 newSize = mRenderersCount;
    if (newSize > mCurrentInstancesSize)
    {
        PROFILER_CPU_NAMED(InstancedBuffers_Resize)

		mCurrentInstancesSize = newSize;
		mCurrentInstancesSize += smInstancesSizeIncrement;

        mGPUMeshBatcher.resize(mCurrentInstancesSize);
        
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mInstanceID).resize(mCurrentInstancesSize);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mObjectID).resize(mCurrentInstancesSize);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mGPUShaderPropertiesInstanceID).resize(mCurrentInstancesSize);
    }

    Core::u32 rendererIndex = 0;
    FOR_RANGE(i, 0, (*mUsedSlots.rbegin())+1)
    {
        Core::WeakPtr<GPURenderItem> renderer = mRenderers[i];
        if(renderer.isValid())
        {
            mGPUMeshBatcher.setInstanceData(rendererIndex, renderer->getRenderSlot().getSlot(), renderer->getGPUShaderPropertiesInstance()->mSlot.getSlot());
            rendererIndex++;
        }
    }

    {
        PROFILER_CPU_NAMED(InstancedBuffers_SetData)
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mInstanceID).setDataArray(mGPUMeshBatcher.getInstanceIDs(), commandBuffer);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mObjectID).setDataArray(mGPUMeshBatcher.getObjectIDs(), commandBuffer);
        mGPUVertexBuffersContainer.getVertexBuffer(GPUShaderDefinitions::VertexInput::mGPUShaderPropertiesInstanceID).setDataArray(mGPUMeshBatcher.getGPUShaderPropertiesInstanceIDs(), commandBuffer);
    }

    mResizeBuffersRequested = false;
}

void GPUInstanceRenderer::setBonesTransformsBuffer(const std::vector<Maths::Matrix4>& transforms)
{
}

bool GPUInstanceRenderer::shouldResize() const
{
    // PERF: possible optimization for dynamic objects: only regenerate buffers when transform changes.
	return mResizeBuffersRequested || !mGPUInstanceRendererData.mIsStatic;
}

void GPUInstanceRenderer::drawCall()
{	
    PROFILER_CPU()
    if(!mRenderers.empty())
    {
//        GET_SYSTEM(GPUInterface).drawElements(GPUDrawPrimitive::TRIANGLES, mGPUInstanceRendererData.mMesh->mIndices.size() * 3, mRenderersCount, true);
        Core::u32 instanceCount = mRenderersCount;
        constexpr Core::u32 firstVertex = 0;
        constexpr Core::u32 vertexOffset = 0;
        constexpr Core::u32 firstInstance = 0;
        const GPUCommandBuffer& vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];
        mGPUContext->drawIndexed(vulkanCommandBuffer.getVkCommandBuffer(), mGPUInstanceRendererData.mMesh->mIndices.size() * 3, instanceCount, firstVertex, vertexOffset, firstInstance);
    }
}

void GPUInstanceRendererManager::terminate()
{
    FOR_MAP(it, mGPUInstanceRenderers)
	{
        it->second->terminate();
	}
}

void GPUInstanceRendererManager::update(GPUContext* gpuContext)
{
    VkCommandBuffer vulkanCommandBuffer = gpuContext->beginSingleTimeCommands();
    FOR_MAP(it, mGPUInstanceRenderers)
	{
        it->second->update(vulkanCommandBuffer);
	}
    gpuContext->endSingleTimeCommands(vulkanCommandBuffer, VK_NULL_HANDLE);
}

bool GPUInstanceRendererManager::addInstanceRenderer(GPUContext* gpuContext, const GPUInstanceRendererData& data)
{
    bool result = false;
    if(!mGPUInstanceRenderers.contains(data))
    {
        PROFILER_CPU_NAMED(init_instanced_mesh)

        mGPUInstanceRenderers.insert_or_assign(data, Core::OwnerPtr<GPUInstanceRenderer>::newObject());
        mGPUInstanceRenderers.at(data)->init(gpuContext, data);
        result = true;
    }

    return result;
}

bool GPUInstanceRendererManager::removeInstanceRenderer(const GPUInstanceRendererData& data)
{
    bool result = false;
    if(mGPUInstanceRenderers.contains(data))
    {
        mGPUInstanceRenderers.at(data)->terminate();
        mGPUInstanceRenderers.erase(data);
        result = true;
    }

    return result;
}

const Core::WeakPtr<GPUInstanceRenderer> GPUInstanceRendererManager::getInstanceRenderer(const GPUInstanceRendererData& data) const
{
    Core::WeakPtr<GPUInstanceRenderer> gpuInstanceRenderer;
    if(mGPUInstanceRenderers.contains(data))
    {
        gpuInstanceRenderer = mGPUInstanceRenderers.at(data);
    }

    return gpuInstanceRenderer;
}

void GPUInstanceRendererRegistry::addInstanceRendererData(const GPUInstanceRendererData& data)
{
    mGPUInstanceRendererDataSet.insert(data);
}

void GPUInstanceRendererRegistry::removeInstanceRendererData(const GPUInstanceRendererData& data)
{
    mGPUInstanceRendererDataSet.erase(data);
}

bool GPUInstanceRendererRegistry::contains(const GPUInstanceRendererData& data) const
{
    return mGPUInstanceRendererDataSet.contains(data);
}
