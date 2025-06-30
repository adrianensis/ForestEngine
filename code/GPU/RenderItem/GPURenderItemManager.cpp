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
            Core::WeakPtr<GPURenderItem> renderItem = mRenderers[i];
            if(renderItem.isValid())
            {
                processRenderer(renderItem);
            }
        }
    }

    // std::for_each(
    //     std::execution::par_unseq,
    //     mUsedSlots.begin(),
    //     mUsedSlots.end(),
    //     [this](u32 i)
    //     {
    //         Core::WeakPtr<GPURenderItem> renderItem = mRenderers[i];
    //         if(renderItem.isValid())
    //         {
    //             processRenderer(renderItem);
    //         }
    //     }
    // );
}

void GPURenderItemManager::processRenderer(Core::WeakPtr<GPURenderItem> renderItem)
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

void GPURenderItemManager::addRenderer(Core::WeakPtr<GPURenderItem> renderItem)
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
    if(renderItem->isStatic())
    {
        setRendererMatrix(renderItem);
        mRenderersStatic.at(renderItem->getRenderSlot().getSlot()) = renderItem;
    }
    else
    {
        mUsedSlots.insert(renderItem->getRenderSlot().getSlot());
        mRenderers.at(renderItem->getRenderSlot().getSlot()) = renderItem;
    }
}

void GPURenderItemManager::removeRenderer(Core::WeakPtr<GPURenderItem> renderItem)
{
    PROFILER_CPU()
    if(renderItem->isStatic())
    {
        mRenderersStatic.at(renderItem->getRenderSlot().getSlot()).invalidate();
    }
    else
    {
        mUsedSlots.erase(renderItem->getRenderSlot().getSlot());
        mRenderers.at(renderItem->getRenderSlot().getSlot()).invalidate();
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

void GPURenderItemManager::setRendererMatrix(Core::WeakPtr<GPURenderItem> renderItem)
{
    PROFILER_CPU()
    if(renderItem->getUpdateMatrix())
    {
        const Matrix4& rendererModelMatrix = renderItem->getRendererModelMatrix();
        CHECK_MSG(mRenderInstancesSlotsManager.checkSlot(renderItem->getRenderSlot()), "Invalid slot!");
        mMatrices.at(renderItem->getRenderSlot().getSlot()) = rendererModelMatrix;
        renderItem->setUpdateMatrix(false);
    }
}