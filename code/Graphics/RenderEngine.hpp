#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "SpacePartition/OcTree.hpp"
#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"

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
    RenderPipelinePBR mRenderPipeline;

public:
	OcTree octree;
};
REGISTER_CLASS(RenderEngine);
