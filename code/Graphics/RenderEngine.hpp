#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/Systems/System.hpp"
#include "Graphics/BatchRenderer/BatchesManager.hpp"
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
    void onResize(u32 width, u32 height);
    
    void addComponent(Ptr<SystemComponent> component) override;
    void removeComponent(Ptr<SystemComponent> component) override;

private:
    void updateInstancesData();

    void swap();
    void render();
private:
	std::vector<Ptr<MeshRenderer>> mRenderers;
    RenderPipelineData mRenderPipelineData;
    RenderPipeline mRenderPipeline;
	BatchesManager mBatchesManager;

	bool mCameraDirtyTranslation = false;

public:
	OcTree octree;
};
REGISTER_CLASS(RenderEngine);
