#pragma once

#include "Core/Minimal.hpp"
#include "GPU/Mesh/GPUMeshBatcher.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshData.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "Graphics/Material/Shader/Shader.hpp"

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
    void update();

private:
    bool shouldRegenerateBuffers() const;

    void initBuffers();
    void resizeMeshBuffers(u32 maxInstances);
    void resizeInstancedBuffers(u32 maxInstances);
    void setMeshBuffers(WeakPtr<const GPUMesh> mesh);
    void setInstancedBuffers();
    void setBonesTransformsBuffer(const std::vector<Matrix4>& transforms);
    void resizeIndicesBuffer(WeakPtr<const GPUMesh> mesh);
    void setIndicesBuffer(WeakPtr<const GPUMesh> mesh);
    void drawCall();

private:
    SlotsManager mRendererSlotsManager;
	std::vector<TComponentHandler<MeshRenderer>> mRenderers;
    u32 mRenderersCount = 0;
    std::set<u32> mUsedSlots;
    inline static const u32 mInitialInstances = 100;

	GPUMeshBatcher mGPUMeshBatcher;
    InstancedMeshData mInstancedMeshData;

    GPUVertexBuffersContainer mGPUVertexBuffersContainer;

	u32 mMaxMeshesThreshold = 0;
	static const u32 smMeshesIncrement = 100;
	bool mRegenerateBuffersRequested = false;

public:
    CRGET(InstancedMeshData)
    CRGET(GPUVertexBuffersContainer)
};
REGISTER_CLASS(InstancedMeshRenderer);
