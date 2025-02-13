#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "SpacePartition/OcTree.hpp"
#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"
#include "Core/ECS/ComponentsManager.hpp"

class RenderEngine : public System, public IComponentsListener
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    void preSceneChanged();
    void postSceneChanged();
    void onResize(u32 width, u32 height);
    
    virtual void onComponentAdded(const ComponentHandler& component) override;
    virtual void onComponentRemoved(const ComponentHandler& component) override;

private:
    RenderPipelineData mRenderPipelineData;
    OwnerPtr<RenderPipelinePBR> mRenderPipeline;
    bool mCompileRequest = true;
public:
	OcTree octree;
};
REGISTER_CLASS(RenderEngine);
