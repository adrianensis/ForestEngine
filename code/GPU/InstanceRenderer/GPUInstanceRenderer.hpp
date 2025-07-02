#pragma once

#include "GPU/Mesh/GPUMeshBatcher.hpp"
#include "GPU/InstanceRenderer/GPUInstanceRendererData.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPUInstanceRenderer
{
public:
    void init(const GPUInstanceRendererData& gpuInstanceRendererData);
    void terminate();

    void render();
    void addRenderer(Core::WeakPtr<GPURenderItem> renderItem);
    void removeRenderer(Core::WeakPtr<GPURenderItem> renderItem);

    bool isEmpty() const { return mRenderersCount == 0; }
    void enable();
    void disable();
    void update(VkCommandBuffer commandBuffer);

private:
    bool shouldResize() const;

    void setMeshBuffers(Core::WeakPtr<const GPUMesh> mesh);
    void setBonesTransformsBuffer(const std::vector<Maths::Matrix4>& transforms);
    void drawCall();

private:
    Core::SlotsManager mRendererSlotsManager;
	std::vector<Core::WeakPtr<GPURenderItem>> mRenderers;
    Core::u32 mRenderersCount = 0;
    std::set<Core::u32> mUsedSlots;
    inline static const Core::u32 smInitialInstancesSize = 100;
    Core::u32 mCurrentInstancesSize = 0;

	GPUMeshBatcher mGPUMeshBatcher;
    GPUInstanceRendererData mGPUInstanceRendererData;

    GPUVertexBuffersContainer mGPUVertexBuffersContainer;

	static const Core::u32 smInstancesSizeIncrement = 100;
	bool mResizeBuffersRequested = false;

public:
    CRGET(GPUInstanceRendererData)
    CRGET(GPUVertexBuffersContainer)
};
REGISTER_CLASS(GPUInstanceRenderer);

class GPUInstanceRendererManager
{
public:
    void terminate();
    void update(Core::Ptr<GPUContext> gpuContext);
    bool addInstanceRenderer(const GPUInstanceRendererData& data);
    bool removeInstanceRenderer(const GPUInstanceRendererData& data);
    const Core::WeakPtr<GPUInstanceRenderer> getInstanceRenderer(const GPUInstanceRendererData& data) const;
private:
    std::unordered_map<GPUInstanceRendererData, Core::OwnerPtr<GPUInstanceRenderer>, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUInstanceRenderers;
    using AA = std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>;
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
