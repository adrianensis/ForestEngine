#ifndef MESHBATCHER_HPP
#define MESHBATCHER_HPP

#include "Graphics/Buffers/VertexBuffer.hpp"

class MeshBatcher: public ObjectBase
{
    GENERATE_METADATA(MeshBatcher)

public:
	MeshBatcher() = default;
    ~MeshBatcher();

    void init(Ptr<const Mesh> prototypeMesh, bool isStatic, bool isInstanced);
    void resize(u32 size);
    void addInstanceMatrix(const Matrix4& modelMatrix);
    void addInstance(const Mesh& meshInstance);
    void drawCall();
    void terminate();
    void enable();
    void disable();

private:
    void clear();
    void generateFacesData(u32 meshesCount);
    void sendDataToGPU();
	
private:
	Ptr<const Mesh> mPrototypeMesh;
	Mesh mMeshBuilder;
	MeshBuffer mMeshBuffer;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;
	u32 mMeshesIndex = 0;

	bool mDataSentToGPU = false;

public:
    GET(PrototypeMesh)
};

#endif