#include "Graphics/RenderEngine.hpp"
#include "GPU/GPUInstance.hpp"
#include "Graphics/Light/Light.hpp"
#include "Engine/Window/WindowManager.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/Module.hpp"
#include "Engine/EngineConfig.hpp"

void RenderEngine::init()
{
	ECManager.addComponentListener<MeshRenderer>(getPtrToThis<RenderEngine>());
	ECManager.addComponentListener<Light>(getPtrToThis<RenderEngine>());

    mRenderPipeline = Core::OwnerPtr<RenderPipelinePBR>::newObject();
    mRenderPipeline->init();
    mRenderPipeline->compile();

	// octree.init(5000);

    GET_SYSTEM(Window::WindowManager).getMainWindow()->addWindowListener(this);
}

void RenderEngine::update()
{
    PROFILER_CPU()
    mRenderPipeline->update();
    mRenderPipeline->render(mRenderPipelineData);

    GET_SYSTEM(GPUInstance).mGPUContext->currentFrame = (GET_SYSTEM(GPUInstance).mGPUContext->currentFrame + 1) % GPUContext::MAX_FRAMES_IN_FLIGHT;
}

void RenderEngine::preSceneChanged()
{
    mCompileRequest = true;
}

void RenderEngine::postSceneChanged()
{
}

void RenderEngine::onResize()
{
//	GET_SYSTEM(GPUInterface).setViewport(0, 0, width, height);
    GET_SYSTEM(GPUInstance).mGPUContext->setWindowResized();
    GET_SYSTEM(GPUInstance).mGPUContext->recreateRenderingObjects();
    mRenderPipeline->onResize();
    EC::TComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();
    camera->onResize();
}

void RenderEngine::terminate()
{
    
    mRenderPipeline->terminate();
}

void RenderEngine::onComponentAdded(const EC::ComponentPtr& component)
{
    if(component->getComponentTypeId() == Core::ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId())
    {
        EC::TComponentPtr<MeshRenderer> renderer = component;
        mRenderPipeline->addRenderer(renderer);

        // if(renderer->getGeometricSpace() == Maths::GeometricSpace::WORLD)
        // {
        //     //octree.addOcTreeElement(Core::WeakPtr<IOcTreeElement>::cast(renderer));
        // }
    }
    else if(component->getComponentTypeId() == Core::ClassManager::getClassMetadata<Light>().mClassDefinition.getId())
    {
        // if(component.getComponent(). <PointLight>())
        // {
        //     mRenderPipelineData.mPointLights.push_back(EC::TComponentPtr<PointLight>(component));
        // }
        // else if(component.getComponent(). <DirectionalLight>())
        // {
        //     mRenderPipelineData.mDirectionalLight = EC::TComponentPtr<DirectionalLight>(component);
        // }
    }
}

void RenderEngine::onComponentRemoved(const EC::ComponentPtr& component)
{
    if(component->getComponentTypeId() == Core::ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId())
    {
        EC::TComponentPtr<MeshRenderer> renderer = component;
        mRenderPipeline->removeRenderer(renderer);
    }
    else if(component->getComponentTypeId() == Core::ClassManager::getClassMetadata<Light>().mClassDefinition.getId())
    {
    }
}