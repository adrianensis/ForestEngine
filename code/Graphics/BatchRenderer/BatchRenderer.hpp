#pragma once

#include "Core/Module.hpp"
#include "Graphics/BatchRenderer/MeshBatcher.hpp"
#include "Graphics/BatchRenderer/BatchData.hpp"
#include "Graphics/GPU/GPUShader.hpp"

class BatchRenderer: public ObjectBase
{
    GENERATE_METADATA(BatchRenderer)

public:
    void init(const BatchData& batchData);
    void terminate();

    void render();
    void addRenderer(Ptr<MeshRenderer> renderer);
	void requestRegenerateBuffers() { mRegenerateBuffersRequested = true; }

private:
    void bindSharedBuffers();
    void enable();
    void disable();
    void processRenderers();
    bool shouldRemoveRenderer(Ptr<const MeshRenderer> renderer);
    void internalRemoveRenderer(std::list<Ptr<MeshRenderer>>::iterator& it);
    void addToVertexBuffer(Ptr<MeshRenderer> renderer);
    bool shouldRegenerateBuffers() const;

private:
	std::list<Ptr<MeshRenderer>> mRenderers;
    OwnerPtr<GPUShader> mShader;
	MeshBatcher mMeshBatcher;
    BatchData mBatchData;

	bool mNewRendererAdded = false;
	bool mRegenerateBuffersRequested = false;

public:
    CRGET(BatchData)
};
