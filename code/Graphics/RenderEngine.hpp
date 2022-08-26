#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP

#include "Core/Module.hpp"
#include "Graphics/Batch/BatchesMap.hpp"
#include "Graphics/Batch/ShapeBatchRenderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Batch/Chunk.hpp"


class RenderEngine : public IEngineSystem, public Singleton<RenderEngine>
{
	GENERATE_METADATA(RenderEngine)

public:
    void init(f32 sceneSize);
    bool frustumTestSphere(const Vector3& center, f32 radius);
    void update();
    void terminate();
    void addComponent(Ptr<IEngineSystemComponent> component);
    Ptr<Chunk> assignChunk(Ptr<Renderer> renderer);
    void assignBatch(Ptr<Renderer> renderer);
    void drawLine(const Line& line, f32 thickness = 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));
    void drawRectangle(const Rectangle& rectangle, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));
    void drawCube(const Cube& cube, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));

private:
    void swap();
    void renderBatches();
    void checkChunks();
private:
	BatchesMap mBatchesMap;

	ShapeBatchRendererMap mShapeBatchRendererMap;
	ShapeBatchRendererMap mShapeBatchRendererMapScreenSpace;

	Ptr<Camera> mCamera;
	bool mCameraDirtyTranslation = false;

	f32 mMinChunkDrawDistance = 0.0f;
	std::vector<OwnerPtr<Chunk>> mChunks;

public:
	RGET_SET(Camera)
	GET(CameraDirtyTranslation)
	GET(MinChunkDrawDistance)
};

#endif