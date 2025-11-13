#pragma once

#include "GPU/RenderItem/GPURenderItem.hpp"

class GPURenderItemManager
{
public:
    virtual void init();
    void update();
    virtual void terminate();
    void addRenderer(Core::WeakPtr<GPURenderItem> renderItem);
    void removeRenderer(Core::WeakPtr<GPURenderItem> renderItem);

    Core::u32 getSize() { return mRenderInstancesSlotsManager.getSize(); }

private:
    void setRendererMatrix(Core::WeakPtr<GPURenderItem> renderItem);
    void processRenderer(Core::WeakPtr<GPURenderItem> renderItem);

private:
    std::vector<Maths::Matrix4> mMatrices;
    Core::SlotsManager mRenderInstancesSlotsManager;
	std::vector<Core::WeakPtr<GPURenderItem>> mRenderers;
    std::set<Core::u32> mUsedSlots;
	std::vector<Core::WeakPtr<GPURenderItem>> mRenderersStatic;
    inline static const Core::u32 mInitialInstances = 1000;

public:
    CRGET(Matrices)
    CRGET(UsedSlots)
};

