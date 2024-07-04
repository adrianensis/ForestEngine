#include "Graphics/RenderEngine.hpp"
#include "Graphics/Renderer/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/Window/WindowManager.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUProgram.hpp"
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
    mRenderPipeline->update();
    mRenderPipeline->render(mRenderPipelineData);
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
	GET_SYSTEM(GPUInterface).setViewport(0, 0, width, height);
    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();
    camera->onResize();
}

void RenderEngine::terminate()
{
	LOG_TRACE()
    
    mRenderPipeline->terminate();
}

void RenderEngine::addSystemComponent(Ptr<SystemComponent> component)
{
	System::addSystemComponent(component);

    if(component->getSystemComponentId() == ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId())
    {
        Ptr<MeshRenderer> renderer = Ptr<MeshRenderer>::cast(component);
        mRenderPipeline->addRenderer(renderer);

        // if(renderer->getGeometricSpace() == GeometricSpace::WORLD)
        // {
        //     //octree.addOcTreeElement(Ptr<IOcTreeElement>::cast(renderer));
        // }
    }
    else if(component->getSystemComponentId() == ClassManager::getClassMetadata<Light>().mClassDefinition.getId())
    {
        if(ClassManager::getDynamicClassMetadata(component.getInternalPointer()).mClassDefinition.getId() == ClassManager::getClassMetadata<PointLight>().mClassDefinition.getId())
        {
            mRenderPipelineData.mPointLights.push_back(Ptr<PointLight>::cast(component));
        }
        else if(ClassManager::getDynamicClassMetadata(component.getInternalPointer()).mClassDefinition.getId() == ClassManager::getClassMetadata<DirectionalLight>().mClassDefinition.getId())
        {
            mRenderPipelineData.mDirectionalLight = Ptr<DirectionalLight>::cast(component);
        }
    }
}

void RenderEngine::removeSystemComponent(Ptr<SystemComponent> component)
{
	System::removeSystemComponent(component);

    if(component->getSystemComponentId() == ClassManager::getClassMetadata<MeshRenderer>().mClassDefinition.getId())
    {
        Ptr<MeshRenderer> renderer = Ptr<MeshRenderer>::cast(component);
        mRenderPipeline->removeRenderer(renderer);
    }
    else if(component->getSystemComponentId() == ClassManager::getClassMetadata<Light>().mClassDefinition.getId())
    {
    }
}

void RenderEngine::swap()
{
	PROFILER_CPU()

	GET_SYSTEM(WindowManager).getMainWindow()->swap();
}