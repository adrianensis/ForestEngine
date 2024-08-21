#include "Graphics/RenderEngine.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/GPU/GPUTexture.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/Window/WindowManager.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/Module.hpp"
#include "Engine/EngineConfig.hpp"

void RenderEngine::init()
{
	LOG_TRACE()

	registerComponentClass(ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId());
	registerComponentClass(ClassManager::getClassMetadata<Light>().mClassDefinition.getId());

    mRenderPipeline = OwnerPtr<RenderPipelinePBR>::newObject();
    mRenderPipeline->init();

	// octree.init(5000);
}

void RenderEngine::update()
{
    PROFILER_CPU()
    // mRenderPipeline->update();
    // mRenderPipeline->render(mRenderPipelineData);
	swap();
}

void RenderEngine::preSceneChanged()
{
	LOG_TRACE()
}

void RenderEngine::postSceneChanged()
{
	LOG_TRACE()
}

void RenderEngine::onResize(u32 width, u32 height)
{
	LOG_TRACE()
//	GET_SYSTEM(GPUInterface).setViewport(0, 0, width, height);
    TypedComponentHandler<Camera> camera = GET_SYSTEM(CameraManager).getCamera();
    camera->onResize();
}

void RenderEngine::terminate()
{
	LOG_TRACE()
    
    mRenderPipeline->terminate();
}

void RenderEngine::addSystemComponent(const ComponentHandler& component)
{
	System::addSystemComponent(component);

    if(component->getSystemComponentId() == ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId())
    {
        TypedComponentHandler<MeshRenderer> renderer = component;
        mRenderPipeline->addRenderer(renderer);

        // if(renderer->getGeometricSpace() == GeometricSpace::WORLD)
        // {
        //     //octree.addOcTreeElement(Ptr<IOcTreeElement>::cast(renderer));
        // }
    }
    else if(component->getSystemComponentId() == ClassManager::getClassMetadata<Light>().mClassDefinition.getId())
    {
        if(component.getComponent().isDerivedClass<PointLight>())
        {
            mRenderPipelineData.mPointLights.push_back(TypedComponentHandler<PointLight>(component));
        }
        else if(component.getComponent().isDerivedClass<DirectionalLight>())
        {
            mRenderPipelineData.mDirectionalLight = TypedComponentHandler<DirectionalLight>(component);
        }
    }
}

void RenderEngine::removeSystemComponent(const ComponentHandler& component)
{
	System::removeSystemComponent(component);

    if(component->getSystemComponentId() == ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId())
    {
        TypedComponentHandler<MeshRenderer> renderer = component;
        mRenderPipeline->removeRenderer(renderer);
    }
    else if(component->getSystemComponentId() == ClassManager::getClassMetadata<Light>().mClassDefinition.getId())
    {
    }
}

void RenderEngine::swap()
{
    PROFILER_CPU()
    PROFILER_GPU()
	GET_SYSTEM(WindowManager).getMainWindow()->swap();
    PROFILER_GPU_COLLECT();
}