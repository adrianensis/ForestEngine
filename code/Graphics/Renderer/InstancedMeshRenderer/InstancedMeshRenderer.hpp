#pragma once

#include "Core/Minimal.hpp"
#include "GPU/Mesh/GPUMeshBatcher.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshData.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/Shader/GPUShader.hpp"

class InstancedMeshRenderer
{
public:
    void init(const InstancedMeshData& instancedMeshData);
    void terminate();

    void render();
    void addRenderer(TComponentHandler<MeshRenderer> renderer);
    void removeRenderer(TComponentHandler<MeshRenderer> renderer);

    bool isEmpty() const { return mRenderersCount == 0; }
    void enable();
    void disable();
    void update(VkCommandBuffer commandBuffer);

private:
    bool shouldResize() const;

    void setMeshBuffers(WeakPtr<const GPUMesh> mesh);
    void setBonesTransformsBuffer(const std::vector<Matrix4>& transforms);
    void drawCall();

private:
    SlotsManager mRendererSlotsManager;
	std::vector<TComponentHandler<MeshRenderer>> mRenderers;
    u32 mRenderersCount = 0;
    std::set<u32> mUsedSlots;
    inline static const u32 smInitialInstancesSize = 100;
    u32 mCurrentInstancesSize = 0;

	GPUMeshBatcher mGPUMeshBatcher;
    InstancedMeshData mInstancedMeshData;

    GPUVertexBuffersContainer mGPUVertexBuffersContainer;

	static const u32 smInstancesSizeIncrement = 100;
	bool mResizeBuffersRequested = false;

public:
    CRGET(InstancedMeshData)
    CRGET(GPUVertexBuffersContainer)
};
REGISTER_CLASS(InstancedMeshRenderer);
