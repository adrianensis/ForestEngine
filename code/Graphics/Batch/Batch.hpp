#ifndef BATCH_HPP
#define BATCH_HPP

#include "Core/Module.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Batch/MeshBatcher.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Batch/BatchData.hpp"

class Batch: public ObjectBase
{
    GENERATE_METADATA(Batch)

public:
	~Batch() override;

    void init(const BatchData& batchData);

    void render();
    void addRenderer(Ptr<Renderer> renderer);
	void forceRegenerateBuffers() { mForceRegenerateBuffers = true; }

private:
    bool processRenderers();
    bool shouldRemoveRenderer(Ptr<Renderer> renderer);
    void internalRemoveRendererFromList(std::list<Ptr<Renderer>>::iterator& it);
    void addToVertexBuffer(Ptr<Renderer> renderer);
    bool shouldRegenerateBuffers() const;

	bool isModelAnimated() const;

	void enableStencil() const;
	void disableStencil() const;

private:
	std::list<Ptr<Renderer>> mRenderers;

	MeshBatcher mMeshBatcher;

    BatchData mBatchData;

	bool mNewRendererAdded = false;
	bool mPendingDrawCall = false;
	bool mForceRegenerateBuffers = false;

};

#endif