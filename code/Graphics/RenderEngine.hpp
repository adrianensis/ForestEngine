#pragma once

#include "Core/Module.hpp"
#include "Graphics/BatchRenderer/BatchesManager.hpp"
#include "Graphics/BatchRenderer/ShapeBatchRenderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "SpacePartition/OcTree.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"

class Light;

class RenderEngine : public System
{
	GENERATE_METADATA(RenderEngine)

public:
    void init();
    void update();
    void preSceneChanged();
    void postSceneChanged();
    void onResize();
    void terminate();
    
    void addComponent(Ptr<SystemComponent> component) override;

    void assignBatch(Ptr<MeshRenderer> renderer);

    void drawLine(const Line& line, f32 thickness = 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));
    void drawRectangle(const Rectangle& rectangle, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));
    void drawCube(const Cube& cube, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));

private:
    void updateMatrices();

    void swap();
    void render();
private:
	std::vector<Ptr<MeshRenderer>> mRenderers;
    RenderPipelineData mRenderPipelineData;
    RenderPipeline mRenderPipeline;
	BatchesManager mBatchesManager;
	ShapeBatchRenderer mShapeBatchRenderer;
	ShapeBatchRenderer mShapeBatchRendererScreenSpace;

	bool mCameraDirtyTranslation = false;

public:
	OcTree octree;
};
