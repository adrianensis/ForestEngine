#ifndef MESHBATCHER_HPP
#define MESHBATCHER_HPP

#include "Graphics/Buffers/MeshBuffer.hpp"

class MeshBatcher: public ObjectBase
{
    GENERATE_METADATA(MeshBatcher)

public:
	MeshBatcher() = default;
    ~MeshBatcher();

    void init(Ptr<const Mesh> prototypeMesh, bool isStatic, bool isInstanced);
    void resize(u32 size);
    void addInstanceMatrix(const Matrix4& modelMatrix);
    void addInstance(Ptr<const Mesh> meshInstance);
    void drawCall();
    void enable();
    void disable();
    bool isAnimated() const;

private:
    void clear();
    void generateFacesData(u32 meshesCount);
    void sendDataToGPU();
	
public:
	Ptr<const Mesh> mPrototypeMesh;

private:
	Mesh mMeshBuilder;
	MeshBuffer mMeshBuffer;

	u32 mMaxMeshesThreshold = 0;
	const u32 mMaxMeshesIncrement = 100;
	u32 mMeshesIndex = 0;

	bool mDataSentToGPU = false;
};

#endif