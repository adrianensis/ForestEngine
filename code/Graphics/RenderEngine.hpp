#pragma once

#include "Engine/Core.hpp"
#include "SpacePartition/OcTree.hpp"
#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Window/Window.hpp"

class RenderEngine : public System::System, public EC::IComponentsListener, public Window::IWindowListener
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    void preSceneChanged();
    void postSceneChanged();
    void onResize() override;
    
    virtual void onComponentAdded(const EC::ComponentPtrBase& component) override;
    virtual void onComponentRemoved(const EC::ComponentPtrBase& component) override;

private:
    RenderPipelineData mRenderPipelineData;
    Core::OwnerPtr<RenderPipelinePBR> mRenderPipeline;
    bool mCompileRequest = true;
public:
	OcTree octree;
};
REGISTER_CLASS(RenderEngine, System);
