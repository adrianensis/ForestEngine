#pragma once

#include "Engine/Core.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "SpacePartition/OcTree.hpp"
#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Window/Window.hpp"

class RenderEngine : public System::System, public EC::IComponentsListener, public Window::IWindowListener
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update(Core::f32 dt);
    void preSceneChanged();
    void postSceneChanged();
    void onResize() override;
    
    virtual void onComponentAdded(EC::Component* component) override;
    virtual void onComponentRemoved(EC::Component* component) override;

private:
    RenderPipelineUpdateData mRenderPipelineUpdateData;
    Core::OwnerPtr<RenderPipelinePBR> mRenderPipeline;
    bool mCompileRequest = true;
    Core::OwnerPtr<CameraManager> mCameraManager;
public:
	OcTree octree;
	GET(CameraManager);
};
REGISTER_CLASS(RenderEngine, System);
