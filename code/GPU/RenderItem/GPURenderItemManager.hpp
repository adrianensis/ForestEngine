#pragma once

#include "Core/Minimal.hpp"
#include "GPU/RenderItem/GPURenderItem.hpp"

class GPURenderItemManager
{
public:
    virtual void init();
    void update();
    virtual void terminate();
    void addRenderer(WeakPtr<GPURenderItem> renderItem);
    void removeRenderer(WeakPtr<GPURenderItem> renderItem);

    u32 getSize() { return mRenderInstancesSlotsManager.getSize(); }

private:
    void setRendererMatrix(WeakPtr<GPURenderItem> renderItem);
    void processRenderer(WeakPtr<GPURenderItem> renderItem);

private:
    std::vector<Matrix4> mMatrices;
    SlotsManager mRenderInstancesSlotsManager;
	std::vector<WeakPtr<GPURenderItem>> mRenderers;
    std::set<u32> mUsedSlots;
	std::vector<WeakPtr<GPURenderItem>> mRenderersStatic;
    inline static const u32 mInitialInstances = 1000;

public:
    CRGET(Matrices)
    CRGET(UsedSlots)
};
REGISTER_CLASS(GPURenderItemManager);
