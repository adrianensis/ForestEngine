#pragma once

#include "Core/Module.hpp"
#include "Graphics/Batch/MeshBatcher.hpp"
#include "Graphics/Batch/BatchData.hpp"
#include "Graphics/GPU/GPUShader.hpp"

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

public:
    void init(const BatchData& batchData);

    void render();
    void addRenderer(Ptr<MeshRenderer> renderer);
	void requestRegenerateBuffers() { mRegenerateBuffersRequested = true; }

private:
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
