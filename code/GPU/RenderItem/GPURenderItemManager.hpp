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

    GPU::u32 getSize() { return mRenderInstancesSlotsManager.getSize(); }

private:
    void setRendererMatrix(GPURenderItem* renderItem);
    void processRenderer(GPURenderItem* renderItem);

private:
    std::vector<Maths::Matrix4> mMatrices;
    Core::SlotsManager mRenderInstancesSlotsManager;
	std::vector<GPURenderItem*> mRenderers;
    std::set<GPU::u32> mUsedSlots;
	std::vector<GPURenderItem*> mRenderersStatic;
    inline static const GPU::u32 mInitialInstances = 1000;

public:
    CRGET(Matrices)
    CRGET(UsedSlots)
};

