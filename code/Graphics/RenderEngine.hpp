#pragma once

#include "Core/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "SpacePartition/OcTree.hpp"
#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"
#include "Engine/Window/Window.hpp"

class RenderEngine : public System::System, public EC::IComponentsListener, public Window::IWindowListener
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    void preSceneChanged();
    void postSceneChanged();
    void onResize() override;
    
    virtual void onComponentAdded(const EC::ComponentPtr& component) override;
    virtual void onComponentRemoved(const EC::ComponentPtr& component) override;

private:
    RenderPipelineData mRenderPipelineData;
    Core::OwnerPtr<RenderPipelinePBR> mRenderPipeline;
    bool mCompileRequest = true;
public:
	OcTree octree;
};
REGISTER_CLASS(RenderEngine);
