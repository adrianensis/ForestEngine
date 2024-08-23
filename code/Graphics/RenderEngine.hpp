#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/System.hpp"
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
    
    void addSystemComponent(const ComponentHandler& component) override;
    void removeSystemComponent(const ComponentHandler& component) override;

private:
    void swap();
private:
    RenderPipelineData mRenderPipelineData;
    OwnerPtr<RenderPipelinePBR> mRenderPipeline;
    bool mCompileRequest = true;
public:
	OcTree octree;
};
REGISTER_CLASS(RenderEngine);
