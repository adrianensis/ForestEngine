#include "GPU/RenderItem/GPURenderItemManager.hpp"

void GPURenderItemManager::init()
{
    PROFILER_CPU()

    mRenderInstancesSlotsManager.init(mInitialInstances * 100);
    mRenderers.resize(mRenderInstancesSlotsManager.getSize());
    mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
    mMatrices.resize(mRenderInstancesSlotsManager.getSize());
}

void GPURenderItemManager::update()
{
	PROFILER_CPU()
    PROFILER_CPU_NAMED(updateRenderers);
    if(!mUsedSlots.empty())
    {
        FOR_RANGE(i, *mUsedSlots.begin(), (*mUsedSlots.rbegin())+1)
        {
            GPURenderItem* renderItem = mRenderers[i];
            if(renderItem)
            {
                processRenderer(renderItem);
            }
        }
    }

    // std::for_each(
    //     std::execution::par_unseq,
    //     mUsedSlots.begin(),
    //     mUsedSlots.end(),
    //     [this](GPU::u32 i)
    //     {
    //         GPURenderItem* renderItem = mRenderers[i];
    //         if(renderItem.isValid())
    //         {
    //             processRenderer(renderItem);
    //         }
    //     }
    // );
}

void GPURenderItemManager::processRenderer(GPURenderItem* renderItem)
{
	PROFILER_CPU()
    if(!renderItem->isStatic())
    {
        // renderer->update();
        setRendererMatrix(renderItem);
    }
}

void GPURenderItemManager::terminate()
{
    mRenderInstancesSlotsManager.reset();
}

void GPURenderItemManager::addRenderer(GPURenderItem* renderItem)
{
    PROFILER_CPU()
    if(mRenderInstancesSlotsManager.isEmpty())
    {
        mRenderInstancesSlotsManager.increaseSize(mInitialInstances);
        mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
        mRenderers.resize(mRenderInstancesSlotsManager.getSize());
        mMatrices.resize(mRenderInstancesSlotsManager.getSize());
    }

    renderItem->setRenderSlot(mRenderInstancesSlotsManager.requestSlot());
    GPU::u32 slot = renderItem->getRenderSlot().getSlot();
    if(renderItem->isStatic())
    {
        setRendererMatrix(renderItem);
        mRenderersStatic[slot] = renderItem;
    }
    else
    {
        mUsedSlots.insert(slot);
        mRenderers[slot] = renderItem;
    }
}

void GPURenderItemManager::removeRenderer(GPURenderItem* renderItem)
{
    PROFILER_CPU()
    GPU::u32 slot = renderItem->getRenderSlot().getSlot();
    if(renderItem->isStatic())
    {
        
        mRenderersStatic[slot] = nullptr;
    }
    else
    {
        mUsedSlots.erase(slot);
        mRenderers[slot] = nullptr;
    }

    mRenderInstancesSlotsManager.freeSlot(renderItem->getRenderSlot());

    // FOR_LIST(it, renderer->getGPURenderItemData().mRenderPassIDs)
    // {
    //     if(mRenderPassMap.contains(*it))
    //     {
    //         mRenderPassMap.at(*it)->removeRenderer(renderer);
    //     }
    // }
}

void GPURenderItemManager::setRendererMatrix(GPURenderItem* renderItem)
{
    PROFILER_CPU()
    if(renderItem->getUpdateMatrix())
    {
        const Maths::Matrix4& rendererModelMatrix = renderItem->getRendererModelMatrix();
        CHECK_MSG(mRenderInstancesSlotsManager.checkSlot(renderItem->getRenderSlot()), "Invalid slot!");
        mMatrices.at(renderItem->getRenderSlot().getSlot()) = rendererModelMatrix;
        renderItem->setUpdateMatrix(false);
    }
}