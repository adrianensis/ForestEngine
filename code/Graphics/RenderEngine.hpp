#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/Systems/System.hpp"
#include "Graphics/BatchRenderer/BatchesManager.hpp"
#include "Graphics/BatchRenderer/ShapeBatchRenderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "SpacePartition/OcTree.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"

class Light;

class RenderEngine : public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    void preSceneChanged();
    void postSceneChanged();
    void onResize();
    
    void addComponent(Ptr<SystemComponent> component) override;
    void removeComponent(Ptr<SystemComponent> component) override;

    void drawLine(const Line& line, f32 thickness = 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));
    void drawRectangle(const Rectangle& rectangle, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));
    void drawCube(const Cube& cube, f32 thickness= 1, bool isWorldSpace = true, Vector4 color = Vector4(1,1,1,1));

private:
    void updateInstancesData();

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
REGISTER_CLASS(RenderEngine);
