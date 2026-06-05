#include "GPU/RenderItem/GPURenderItemManager.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/CoreMacros.hpp"

void GPURenderItemManager::init()
{
    PROFILER_CPU()

    mSize = mInitialInstances * 100;
    mRenderers.resize(mSize);
    mMatrices.resize(mSize);
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
}

void GPURenderItemManager::addRenderer(GPURenderItem* renderItem)
{
    PROFILER_CPU()
    if(mRenderItemsCount == mSize)
    {
        mSize += mInitialInstances;
        mRenderers.resize(mSize);
        mMatrices.resize(mSize);
    }

    GPU::u32 slot = 0;
    bool found = false;
    FOR_RANGE(i, 0, mSize)
    {
        if(mRenderers[i] == nullptr)
        {
            slot = i;
            found = true;
        }
    }

    CHECK_MSG(found, "No slot avaliable for renderer.")

    renderItem->setRenderSlot(slot);
    
    mRenderers[slot] = renderItem;
    
    if(renderItem->isStatic())
    {
        setRendererMatrix(renderItem);
    }
    else
    {
        mUsedSlots.insert(slot);
    }

    mRenderItemsCount++;
}

void GPURenderItemManager::removeRenderer(GPURenderItem* renderItem)
{
    PROFILER_CPU()
    GPU::u32 slot = renderItem->getRenderSlot();
    if(renderItem->isStatic())
    {
        
    }
    else
    {
        mUsedSlots.erase(slot);
    }

    mRenderers[slot] = nullptr;

    // FOR_LIST(it, renderer->getGPURenderItemData().mRenderPassIDs)
    // {
    //     if(mRenderPassMap.contains(*it))
    //     {
    //         mRenderPassMap.at(*it)->removeRenderer(renderer);
    //     }
    // }
    mRenderItemsCount--;
}

void GPURenderItemManager::setRendererMatrix(GPURenderItem* renderItem)
{
    PROFILER_CPU()
    if(renderItem->getUpdateMatrix())
    {
        const Maths::Matrix4& rendererModelMatrix = renderItem->getRendererModelMatrix();
        CHECK_MSG(mRenderers.at(renderItem->getRenderSlot()), "Invalid slot!");
        mMatrices.at(renderItem->getRenderSlot()) = rendererModelMatrix;
        renderItem->setUpdateMatrix(false);
    }
}