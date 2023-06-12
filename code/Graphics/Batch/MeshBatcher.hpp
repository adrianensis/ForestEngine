#pragma once

#include "Graphics/Buffers/GPUMeshBuffer.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class MeshBatcher: public ObjectBase
{
    GENERATE_METADATA(MeshBatcher)

public:
	MeshBatcher() = default;

    void init(Ptr<const Mesh> prototypeMesh, bool isStatic, bool isInstanced, bool useVertexColor);
    void resize(u32 size);
    void addInstance(const Matrix4& modelMatrix,Ptr<const Mesh> meshInstance);
    void drawCall();
    void enable();
    void disable();
    bool isAnimated() const;
    const GPUBuffersLayout& getGPUBuffersLayout() const { return mGPUMeshBuffer.mBuffersLayout; }

private:
    void addDataToBuffers(Ptr<const Mesh> meshInstance);
    void clear();
    void generateFacesData(u32 meshesCount);
    void sendDataToGPU();
	
public:
	Ptr<const Mesh> mPrototypeMesh;
	GPUMeshBuffer mGPUMeshBuffer;

private:
	Mesh mMeshBuilder;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;
	u32 mMeshesIndex = 0;

	bool mDataSentToGPU = false;
};
