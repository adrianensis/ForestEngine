#pragma once

#include "GPU/RenderItem/GPURenderItem.hpp"

class GPURenderItemManager
{
public:
    virtual void init();
    void update();
    virtual void terminate();
    void addRenderer(GPURenderItem* renderItem);
    void removeRenderer(GPURenderItem* renderItem);

    GPU::u32 getSize() { return mSize; }

private:
    void setRendererMatrix(GPURenderItem* renderItem);
    void processRenderer(GPURenderItem* renderItem);

private:
    GPU::u32 mSize = 0;
    GPU::u32 mRenderItemsCount = 0;
    std::vector<Maths::Matrix4> mMatrices;
    // TODO: can I make this not-pointers?
	std::vector<GPURenderItem*> mRenderers;
    std::set<GPU::u32> mUsedSlots;
    inline static const GPU::u32 mInitialInstances = 1000;

public:
    CRGET(Matrices)
    CRGET(UsedSlots)
};

