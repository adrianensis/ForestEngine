#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/BatchRenderer/MeshBatcher.hpp"
#include "Graphics/BatchRenderer/BatchData.hpp"
#include "Graphics/GPU/GPUProgram.hpp"

class BatchRenderer: public ObjectBase
{
    

public:
    void init(const BatchData& batchData);
    void terminate();

    void render();
    void onAddRenderer(Ptr<MeshRenderer> renderer);
    void onRemoveRenderer(Ptr<MeshRenderer> renderer);

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
REGISTER_CLASS(BatchRenderer);
