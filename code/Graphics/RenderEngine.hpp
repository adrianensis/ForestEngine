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
    void initCameraManager(Core::WeakPtr<CameraManager> cameraManager);
    void update(Core::f32 dt);
    void preSceneChanged();
    void postSceneChanged();
    void onResize(Window::Window* window) override;
    
    virtual void onComponentAdded(EC::Component* component) override;
    virtual void onComponentRemoved(EC::Component* component) override;

private:
    RenderPipelineUpdateData mRenderPipelineUpdateData;
    Core::OwnerPtr<RenderPipelinePBR> mRenderPipeline;
    bool mCompileRequest = true;
    Core::WeakPtr<CameraManager> mCameraManager;
public:
	OcTree octree;
};
REGISTER_CLASS(RenderEngine, System);
