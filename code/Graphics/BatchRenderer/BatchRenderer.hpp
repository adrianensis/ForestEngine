#pragma once

#include "Core/Module.hpp"
#include "Graphics/BatchRenderer/MeshBatcher.hpp"
#include "Graphics/BatchRenderer/BatchData.hpp"
#include "Graphics/GPU/GPUProgram.hpp"

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
    void updateBuffers();
    bool shouldRegenerateBuffers() const;

private:
	std::vector<Ptr<MeshRenderer>> mRenderers;
    OwnerPtr<GPUProgram> mShader;
	MeshBatcher mMeshBatcher;
    BatchData mBatchData;

	bool mRegenerateBuffersRequested = false;

public:
    CRGET(BatchData)
};
