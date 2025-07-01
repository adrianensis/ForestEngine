#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "SpacePartition/OcTree.hpp"
#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Window/Window.hpp"

class RenderEngine : public Core::System, public IComponentsListener, public Window::IWindowListener
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    void preSceneChanged();
    void postSceneChanged();
    void onResize() override;
    
    virtual void onComponentAdded(const ComponentPtr& component) override;
    virtual void onComponentRemoved(const ComponentPtr& component) override;

private:
    RenderPipelineData mRenderPipelineData;
    Core::OwnerPtr<RenderPipelinePBR> mRenderPipeline;
    bool mCompileRequest = true;
public:
	OcTree octree;
};
REGISTER_CLASS(RenderEngine);
