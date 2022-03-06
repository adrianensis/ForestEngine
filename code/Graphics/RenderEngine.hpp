#pragma once

// CPP_IGNORE

#include "Core/Module.hpp"
#include "Graphics/Optimizations/BatchesMap.hpp"
#include "Graphics/ShapeBatchRenderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Optimizations/Chunk.hpp"

class RenderEngine : public IEngineSystem, public Singleton<RenderEngine>
{
	GENERATE_METADATA(RenderEngine)

private:
	BatchesMap mBatchesMap;

	ShapeBatchRendererMap mShapeBatchRendererMap;
	ShapeBatchRendererMap mShapeBatchRendererMapScreenSpace;

	Camera* mCamera = nullptr;
	bool mCameraDirtyTranslation = false;

	f32 mMinChunkDrawDistance = 0.0f;
	std::vector<Chunk *> mChunks;

	void checkChunks();
	void renderBatches();
	void swap();

public:
	void init(f32 sceneSize);
	void update();	  // render
	void terminate();

	void addComponent(IEngineSystemComponent *component) override;
	Chunk *assignChunk(Renderer * renderer);
	bool frustumTestSphere(const Vector3& center, f32 radius);

	void drawLine(const Line& line, f32 thickness = 1, bool isWorldSpace = true, Vector4 color = Vector4(1, 1, 1, 1));
	void drawRectangle(const Rectangle& rectangle, f32 thickness = 1, bool isWorldSpace = true, Vector4 color = Vector4(1, 1, 1, 1));
	//public: void drawRectangle(const Rectangle& rectangle, f32 thickness = 1, bool isWorldSpace = true, Vector4 color = Vector4(1, 1, 1, 1));

	GET_SET(Camera)
	GET(CameraDirtyTranslation)
	GET(MinChunkDrawDistance)
};