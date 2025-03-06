#include "Graphics/RenderEngine.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "GPU/GPUInstance.hpp"
#include "Graphics/Light/Light.hpp"
#include "Core/Window/WindowManager.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/Module.hpp"
#include "Engine/EngineConfig.hpp"

void RenderEngine::init()
{
	LOG_TRACE()

	ComponentsManager::getInstance().addComponentListener<MeshRenderer>(getPtrToThis<RenderEngine>());
	ComponentsManager::getInstance().addComponentListener<Light>(getPtrToThis<RenderEngine>());

    mRenderPipeline = OwnerPtr<RenderPipelinePBR>::newObject();
    mRenderPipeline->init();
    mRenderPipeline->compile();

	// octree.init(5000);

    GET_SYSTEM(WindowManager).getMainWindow()->addWindowListener(this);
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
	LOG_TRACE()
    mCompileRequest = true;
}

void RenderEngine::postSceneChanged()
{
	LOG_TRACE()
}

void RenderEngine::onResize()
{
	LOG_TRACE()
//	GET_SYSTEM(GPUInterface).setViewport(0, 0, width, height);
    GET_SYSTEM(GPUInstance).mGPUContext->setWindowResized();
    GET_SYSTEM(GPUInstance).mGPUContext->recreateRenderingObjects();
    mRenderPipeline->onResize();
    TComponentHandler<Camera> camera = GET_SYSTEM(CameraManager).getCamera();
    camera->onResize();
}

void RenderEngine::terminate()
{
	LOG_TRACE()
    
    mRenderPipeline->terminate();
}

void RenderEngine::onComponentAdded(const ComponentHandler& component)
{
    if(component->getComponentTypeId() == ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId())
    {
        TComponentHandler<MeshRenderer> renderer = component;
        mRenderPipeline->addRenderer(renderer);

        // if(renderer->getGeometricSpace() == GeometricSpace::WORLD)
        // {
        //     //octree.addOcTreeElement(WeakPtr<IOcTreeElement>::cast(renderer));
        // }
    }
    else if(component->getComponentTypeId() == ClassManager::getClassMetadata<Light>().mClassDefinition.getId())
    {
        // if(component.getComponent(). <PointLight>())
        // {
        //     mRenderPipelineData.mPointLights.push_back(TComponentHandler<PointLight>(component));
        // }
        // else if(component.getComponent(). <DirectionalLight>())
        // {
        //     mRenderPipelineData.mDirectionalLight = TComponentHandler<DirectionalLight>(component);
        // }
    }
}

void RenderEngine::onComponentRemoved(const ComponentHandler& component)
{
    if(component->getComponentTypeId() == ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId())
    {
        TComponentHandler<MeshRenderer> renderer = component;
        mRenderPipeline->removeRenderer(renderer);
    }
    else if(component->getComponentTypeId() == ClassManager::getClassMetadata<Light>().mClassDefinition.getId())
    {
    }
}