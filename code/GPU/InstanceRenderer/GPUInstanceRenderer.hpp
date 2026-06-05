#pragma once

#include "GPU/Mesh/GPUMeshBatcher.hpp"
#include "GPU/InstanceRenderer/GPUInstanceRendererData.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPUInstanceRenderer
{
public:
    void init(GPUContext* gpuContext, const GPUInstanceRendererData& gpuInstanceRendererData);
    void terminate();

    void render();
    void addRenderer(GPURenderItem* renderItem);
    void removeRenderer(GPURenderItem* renderItem);

    bool isEmpty() const { return mRenderersCount == 0; }
    void enable();
    void disable();
    void update(VkCommandBuffer commandBuffer);

private:
    bool shouldResize() const;

    void setMeshBuffers(const GPUMesh* mesh);
    void setBonesTransformsBuffer(const std::vector<Maths::Matrix4>& transforms);
    void drawCall();

private:
    GPUContext* mGPUContext = nullptr;
    // Core::SlotsManager mRendererSlotsManager;
	std::vector<GPURenderItem*> mRenderers;
    GPU::u32 mRenderersCount = 0;
    std::set<GPU::u32> mUsedSlots;
    inline static const GPU::u32 smInitialInstancesSize = 1000;
    GPU::u32 mCurrentInstancesSize = 0;

	GPUMeshBatcher mGPUMeshBatcher;
    GPUInstanceRendererData mGPUInstanceRendererData;

    GPUVertexBuffersContainer mGPUVertexBuffersContainer;

	static const GPU::u32 smInstancesSizeIncrement = 100;
	bool mResizeBuffersRequested = false;

public:
    CRGET(GPUInstanceRendererData)
    CRGET(GPUVertexBuffersContainer)
};


class GPUInstanceRendererManager
{
public:
    void terminate();
    void update(GPUContext* gpuContext);
    bool addInstanceRenderer(GPUContext* gpuContext, const GPUInstanceRendererData& data);
    bool removeInstanceRenderer(const GPUInstanceRendererData& data);
    GPUInstanceRenderer* getInstanceRenderer(const GPUInstanceRendererData& data) const;
private:
    std::unordered_map<GPUInstanceRendererData, GPUInstanceRenderer*, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUInstanceRenderers;
};

class GPUInstanceRendererRegistry
{
public:
    void addInstanceRendererData(const GPUInstanceRendererData& data);
    void removeInstanceRendererData(const GPUInstanceRendererData& data);
    bool contains(const GPUInstanceRendererData& data) const;
private:
    std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUInstanceRendererDataSet;
public:
    CRGET(GPUInstanceRendererDataSet)
};
