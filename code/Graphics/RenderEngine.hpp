#pragma once

#include "Core/Module.hpp"
#include "Graphics/Batch/BatchesMap.hpp"
#include "Graphics/Batch/ShapeBatchRenderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "SpacePartition/OcTree.hpp"

class RenderEngine : public EngineSystem
{
	GENERATE_METADATA(RenderEngine)

public:
    void init(f32 sceneSize);
    bool frustumTestSphere(const Vector3& center, f32 radius);
    void update();
    void terminate();
    
    void addComponent(Ptr<EngineSystemComponent> component) override;

    void assignBatch(Ptr<MeshRenderer> renderer);

    void drawLine(const Line& line, f32 thickness = 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));
    void drawRectangle(const Rectangle& rectangle, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));
    void drawCube(const Cube& cube, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));

private:
    void swap();
    void renderBatches();
private:
	BatchesMap mBatchesMap;

	ShapeBatchRenderer mShapeBatchRenderer;
	ShapeBatchRenderer mShapeBatchRendererScreenSpace;

	bool mCameraDirtyTranslation = false;

public:
	Ptr<Camera> mCamera;
	OcTree octree;
};
