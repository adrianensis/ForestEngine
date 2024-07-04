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
    virtual void update() override;
    void preSceneChanged();
    void postSceneChanged();
    void onResize(u32 width, u32 height);
    
    void addSystemComponent(Ptr<SystemComponent> component) override;
    void removeSystemComponent(Ptr<SystemComponent> component) override;

private:
    void swap();
private:
    RenderPipelineData mRenderPipelineData;
    OwnerPtr<RenderPipelinePBR> mRenderPipeline;

public:
	OcTree octree;
};
REGISTER_CLASS(RenderEngine);
